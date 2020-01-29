#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include "Misc.hpp"
#include "Ordering.hpp"
#include <ostream>
#include "RNG.hpp"
#include "RunOptions.hpp"
#include <vector>

namespace TwinPeaks
{

/*
* An object of this class is the current state of a sampler.
*/
template<typename T>
class Sampler
{
    private:

        // Run options
        RunOptions run_options;

        // Iteration counter
        int iteration;

        // Validity flag - can the sampler be run?
        bool valid;

        // The particles
        std::vector<T> particles;

        // The particles' scalars, tiebreakers, and ranks
        std::vector<double> xs, ys;
        std::vector<int> x_indices, y_indices;
        std::vector<int> x_ranks, y_ranks;

        // Positions along space filling curve
        std::vector<double> ds;

        // Forbidden rectangles
        std::list<double> forbidden_xs;
        std::list<double> forbidden_ys;
        void add_forbidden_rectangle(double x, double y);

        // Index of worst particle
        int worst;

        // Compute all ordering-related stuff
        void compute_orderings();

        // Write dead particle output to files
        void write_output() const;

        // Do one NS iteration
        void advance(RNG& rng, bool last_iteration=false);

        // Test a position wrt the forbidden rectangles
        double test(double x, double y) const;

    public:

        // Default constructor disabled
        Sampler() = delete;

        // Constructor where you provide the number of particles
        // and an RNG for particle initialisation
        Sampler(RunOptions _run_options, RNG& rng);

        // Run to target depth
        void run(RNG& rng);
};


/* IMPLEMENTATIONS FOLLOW */

template<typename T>
Sampler<T>::Sampler(RunOptions _run_options, RNG& rng)
:run_options(std::move(_run_options))
,iteration(1)
,valid(true)
,particles(run_options.num_particles)
,xs(run_options.num_particles)
,ys(run_options.num_particles)
,x_indices(run_options.num_particles)
,y_indices(run_options.num_particles)
,x_ranks(run_options.num_particles)
,y_ranks(run_options.num_particles)
,ds(run_options.num_particles)
,forbidden_xs()
,forbidden_ys()
{
    // Generate particles from the prior.
    std::cout << "# Generating " << run_options.num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;
    for(int i=0; i<run_options.num_particles; ++i)
    {
        particles[i].from_prior(rng);
        auto scalars = particles[i].get_scalars();
        std::tie(xs[i], ys[i]) = scalars;
    }
    compute_orderings();
    std::cout << "done.\n#" << std::endl;

    // Save a record of the run options
    run_options.save();
}


template<typename T>
void Sampler<T>::compute_orderings()
{
    // Ranks of all particles
    std::tie(x_indices, x_ranks) = indices_and_ranks(xs);
    std::tie(y_indices, y_ranks) = indices_and_ranks(ys);

    // Map ranks to total order
    for(int i=0; i<run_options.num_particles; ++i)
        ds[i] = d(x_ranks[i], y_ranks[i], true);

    // Find worst particle
    worst = 0;
    for(int i=1; i<run_options.num_particles; ++i)
        if(ds[i] < ds[worst])
            worst = i;
}

template<typename T>
void Sampler<T>::run(RNG& rng)
{
    if(!valid)
    {
        std::cerr << "Cannot run sampler. It's probably already been done.";
        std::cerr << std::endl;
        return;
    }
    int iterations = static_cast<int>(run_options.num_particles
                                                *run_options.depth);
    for(int i=0; i<iterations; ++i)
        advance(rng, i==iterations-1);
    valid = false;
}

template<typename T>
double Sampler<T>::test(double x, double y) const
{
    auto it1 = forbidden_xs.begin();
    auto it2 = forbidden_ys.begin();
    while(it1 != forbidden_xs.end() && it2 != forbidden_xs.end())
    {
        if(x < *it1 && y < *it2)
            return false;
        ++it1;
        ++it2;
    }
    return true;
}


template<typename T>
void Sampler<T>::add_forbidden_rectangle(double x, double y)
{
    // First, prune redundant old rectangles
    auto it1 = forbidden_xs.begin();
    auto it2 = forbidden_ys.begin();
    while(it1 != forbidden_xs.end() && it2 != forbidden_ys.end())
    {
        if(x >= *it1 && y >= *it2)
        {
            it1 = forbidden_xs.erase(it1);
            it2 = forbidden_ys.erase(it2);
        }

        ++it1;
        ++it2;
    }

    forbidden_xs.push_front(x);
    forbidden_ys.push_front(y);
}



template<typename T>
void Sampler<T>::advance(RNG& rng, bool last_iteration)
{
    bool output_message = run_options.num_particles <= 100 ||
                                iteration % run_options.num_particles == 0;

    // Progress message
    if(output_message)
    {
        std::cout << std::setprecision(12);
        std::cout << "# Iteration " << iteration << ". ";
        std::cout << "Depth ~= " << (double)iteration/run_options.num_particles << " nats.\n";
        std::cout << "# Standard deviation of sum of ranks = ";
        double tot = 0.0;
        double tot_sq = 0.0;
        int n = run_options.num_particles;
        for(int i=0; i<n; ++i)
        {
            tot += (x_ranks[i] + y_ranks[i]);
            tot_sq += pow(x_ranks[i] + y_ranks[i], 2);
        }
        std::cout << sqrt(tot_sq/n - pow(tot/n, 2)) << '.';
        std::cout << std::endl;

        std::cout << "# Number of forbidden rectangles = ";
        std::cout << forbidden_xs.size() << "." << std::endl;
    }
    // Save worst particle
    if(output_message)
        std::cout << "#    Saving worst particle..." << std::flush;
    write_output();

    if(output_message)
    {
        std::cout << "done. ";
        std::cout << "Scalars = (" << xs[worst] << ", ";
        std::cout << ys[worst] << ")." << std::endl;
    }

    if(last_iteration)
        return;

    // Add new forbidden rectangles
    int ix, iy;
    for(iy=0; true; ++iy)
    {
        for(ix=0; true; ++ix)
        {
            if(d(ix, iy) > ds[worst])
                break;

            if(d(ix+1, iy) > ds[worst])
                add_forbidden_rectangle(xs[x_indices[ix]], ys[y_indices[iy]]);
        }

        if(ix == 0)
            break;
    }

    if(output_message)
        std::cout << "#    Generating replacement particle..." << std::flush;

    // Generate new particle
    // First, clone
    int copy = 0;
    do
    {
        copy = rng.rand_int(run_options.num_particles);
    }while(run_options.num_particles > 1 && copy == worst);
    T new_particle = particles[copy];
    double new_x = xs[copy];
    double new_y = ys[copy];

    // Now do Metropolis
    int accepted = 0;
    for(int i=0; i<run_options.mcmc_steps; ++i)
    {
        // Make proposal

        // Particle
        auto proposal = new_particle;
        double logH = proposal.perturb(rng);

        // Get its scalars
        double proposal_x, proposal_y;
        std::tie(proposal_x, proposal_y) = proposal.get_scalars();

        if(test(proposal_x, proposal_y) &&
           (rng.rand() <= exp(logH)))
        {
            new_particle = proposal;
            new_x = proposal_x;
            new_y = proposal_y;
            ++accepted;
        }

    }

    // Insert new particle and recompute stuff
    particles[worst] = new_particle;
    xs[worst] = new_x;
    ys[worst] = new_y;

    if(output_message)
    {
        std::cout << "done. ";
        std::cout << "Metropolis acceptance rate = ";
        std::cout << accepted << '/' << run_options.mcmc_steps << '.' << std::endl;
    }

    // Compute new orderings.
    if(output_message)
        std::cout << "#    Computing new orderings..." << std::flush;

    compute_orderings();

    if(output_message)
        std::cout << "done." << std::endl;

    // Increment iteration counter
    ++iteration;

    if(output_message)
        std::cout << '#' << std::endl;
}


template<typename T>
void Sampler<T>::write_output() const
{
    // Open the output file
    std::fstream fout;
    if(iteration == 1)
        fout.open("output/scalars.csv", std::ios::out);
    else
        fout.open("output/scalars.csv", std::ios::out | std::ios::app);

    // Set precision
    fout << std::setprecision(12);

    // Output scalars
    fout << iteration << ',';
    fout << xs[worst] << ',';
    fout << ys[worst] << std::endl;

    // Close the output file
    fout.close();

    // Now output entire particle
    if(iteration % run_options.thin != 0)
        return;

    if(iteration / run_options.thin == 1)
        fout.open("output/particles.csv", std::ios::out);
    else
        fout.open("output/particles.csv", std::ios::out | std::ios::app);
    fout << iteration << ',';
    fout << particles[worst].render() << std::endl;
    fout.close();
}

} // namespace

#endif

