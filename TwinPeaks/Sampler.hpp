#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include <fstream>
#include <iomanip>
#include <iostream>
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
        std::vector<int> x_ranks, y_ranks;

        // Positions along space filling curve
        std::vector<double> ds;

        // Index of worst particle
        int worst;

        // Compute all ordering-related stuff
        void compute_orderings();

        // Write dead particle output to files
        void write_output() const;

        // Do one NS iteration
        void advance(RNG& rng, bool last_iteration=false);

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
,x_ranks(run_options.num_particles)
,y_ranks(run_options.num_particles)
,ds(run_options.num_particles)
{
    // Generate particles from the prior.
    std::cout << "Generating " << run_options.num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;
    for(int i=0; i<run_options.num_particles; ++i)
    {
        particles[i].from_prior(rng);
        auto scalars = particles[i].get_scalars();
        std::tie(xs[i], ys[i]) = scalars;
    }
    compute_orderings();
    std::cout << "done.\n" << std::endl;

    // Save a record of the run options
    run_options.save();
}


template<typename T>
void Sampler<T>::compute_orderings()
{
    // Ranks of all particles
    x_ranks = compute_ranks(xs);
    y_ranks = compute_ranks(ys);

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
void Sampler<T>::advance(RNG& rng, bool last_iteration)
{
    bool output_message = run_options.num_particles <= 100 ||
                                iteration % run_options.num_particles == 0;

    // Progress message
    if(output_message)
    {
        std::cout << std::setprecision(12);
        std::cout << "Iteration " << iteration << ". ";
        std::cout << "Depth ~= " << (double)iteration/run_options.num_particles << " nats.";
        std::cout << std::endl;
    }
    // Save worst particle
    if(output_message)
        std::cout << "    Saving worst particle..." << std::flush;
    write_output();

    if(output_message)
    {
        std::cout << "done. ";
        std::cout << "Scalars = (" << xs[worst] << ", ";
        std::cout << ys[worst] << ")." << std::endl;
    }

    if(last_iteration)
        return;

    if(output_message)
        std::cout << "    Generating replacement particle..." << std::flush;

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

        // Compute ranks
        int xr = 0;
        int yr = 0;
        for(int j=0; j<run_options.num_particles; ++j)
        {
            if(xs[j] < proposal_x)
                ++xr;
            if(ys[j] < proposal_y)
                ++yr;
        }

        int proposal_d = d(xr, yr);
        if((proposal_d >= ds[worst]) && 
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
        std::cout << "    Computing new orderings..." << std::flush;

    compute_orderings();

    if(output_message)
        std::cout << "done." << std::endl;

    // Increment iteration counter
    ++iteration;

    if(output_message)
        std::cout << std::endl;
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

