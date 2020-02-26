#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include "Constraints.hpp"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
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
        RunOptions options;

        // Iteration counter and depth
        int iteration;

        // Flip x and y ranks in total ordering?
        bool flip;
        std::shared_ptr<Sampler<T>> fork; // Forked sampler with flipped order

        // The particles
        std::vector<T> particles;

        // The particles' scalars, tiebreakers, and ranks
        std::vector<double> xs, ys;
        std::vector<int> x_indices, y_indices;
        std::vector<int> x_ranks, y_ranks;

        // Quality (target scalar)
        std::vector<double> Qs;

        // Forbidden rectangles
        Constraints constraints;

        // Index of worst particle
        int worst;

        // Compute all ordering-related stuff
        void compute_orderings();

        // Write dead particle output to files
        void write_output() const;

        // Do one NS iteration
        void advance(RNG& rng, bool final_iteration=false);

        // Does the fork exist?
        bool fork_exists() const;

        // Fork the sampler
        void create_fork();

        // Sampler id. 0=pre-fork, 1 and 2 are post-fork
        int sampler_id() const;

    public:

        // Constructor where you provide the number of particles
        // and an RNG for particle initialisation
        Sampler(RunOptions _options, RNG& rng);

        // Run to target depth
        void run(RNG& rng);
};


/* IMPLEMENTATIONS FOLLOW */

template<typename T>
Sampler<T>::Sampler(RunOptions _options, RNG& rng)
:options(std::move(_options))
,iteration(1)
,flip(false)
,fork(nullptr)
,particles(options.num_particles)
,xs(options.num_particles)
,ys(options.num_particles)
,x_indices(options.num_particles)
,y_indices(options.num_particles)
,x_ranks(options.num_particles)
,y_ranks(options.num_particles)
,Qs(options.num_particles)
{
    // Generate particles from the prior.
    std::cout << "# Generating " << options.num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;
    for(int i=0; i<options.num_particles; ++i)
    {
        particles[i].from_prior(rng);
        auto scalars = particles[i].get_scalars();
        std::tie(xs[i], ys[i]) = scalars;
    }
    compute_orderings();
    std::cout << "done.\n#" << std::endl;

    // Save a record of the run options
    options.save();
}

template<typename T>
bool Sampler<T>::fork_exists() const
{
    // From main sampler, a non-null 'fork' object indicates the existence
    // of the forked sampler.

    // From the forked sampler, the fact that 'flip' is true indicates that
    // we are in the forked sampler.

    if(fork.get() != nullptr)
        return true;
    if(flip)
        return true;

    return false;
}

template<typename T>
int Sampler<T>::sampler_id() const
{
    if(!fork_exists())
        return 0;
    if(flip)
        return 2;
    return 1;
}


template<typename T>
void Sampler<T>::compute_orderings()
{
    // Ranks of all particles
    std::tie(x_indices, x_ranks) = indices_and_ranks(xs);
    std::tie(y_indices, y_ranks) = indices_and_ranks(ys);

    // Map ranks to total order
    for(int i=0; i<options.num_particles; ++i)
        Qs[i] = Q(x_ranks[i], y_ranks[i], true, flip);

    // Find worst particle
    worst = 0;
    for(int i=1; i<options.num_particles; ++i)
        if(Qs[i] < Qs[worst])
            worst = i;
}

template<typename T>
void Sampler<T>::run(RNG& rng)
{
    int iterations = static_cast<int>(options.num_particles
                                                *options.depth);
    for(int i=0; i<iterations; ++i)
    {
        advance(rng, i==iterations-1);
        if(fork_exists())
            fork->advance(rng, i==iterations-1);        
    }
}


template<typename T>
void Sampler<T>::create_fork()
{
    if(fork_exists())
        throw std::logic_error("Can't fork sampler - it's already happened.");

    std::cout << "# Forking sampler." << std::endl;
    fork = std::shared_ptr<Sampler<T>>(new Sampler<T>(*this));
    fork->flip = true;

    // Recompute orderings for the fork
    fork->compute_orderings();
}


template<typename T>
void Sampler<T>::advance(RNG& rng, bool final_iteration)
{
    bool output_message = options.num_particles <= 100 ||
                                iteration % options.num_particles == 0;

    // Number of off-diagonal points
    std::vector<int> temp(options.num_particles);
    double mean = 0.0;
    for(int i=0; i<options.num_particles; ++i)
    {
        temp[i] = x_ranks[i] + y_ranks[i] - (options.num_particles-1);
        mean += temp[i]/options.num_particles;
    }
    double mean_abs_dev = 0.0;
    for(int i=0; i<options.num_particles; ++i)
        mean_abs_dev += std::abs(temp[i] - mean)/options.num_particles;

    // Test for degeneracy of particle ranks
    if(mean_abs_dev == 0.0 && !fork_exists())
    {
        // Create the fork
        create_fork();
    }

    // Progress message
    if(output_message)
    {
        std::cout << std::setprecision(12);
        std::cout << "# Sampler " << sampler_id() << ", ";
        std::cout << "iteration " << iteration << ". ";
        std::cout << "Depth ~= " << double(iteration)/options.num_particles << " nats.\n";

        std::cout << "# Number of forbidden rectangles = ";
        std::cout << constraints.size() << "." << std::endl;

        std::cout << "# Spread around diagonal = ";
        std::cout << mean_abs_dev << '.' << std::endl;
    }

    // Save worst particle
    write_output();

    if(output_message)
    {
        std::cout << "#    Worst particle's scalars = (" << xs[worst] << ", ";
        std::cout << ys[worst] << ")." << std::endl;
    }

    if(final_iteration)
    {
        std::cout << '#' << std::endl;
        return;
    }

    // Add new forbidden rectangles
    const int& N=options.num_particles;
    int xr;
    for(int yr=N-1; yr>=0; --yr)
    {
        std::vector<std::tuple<double, double>> rects;
        for(xr=0; xr<N; ++xr)
        {
            if(Q(xr, yr, false, flip) < Qs[worst])
            {
                rects.emplace_back(xs[x_indices[xr]],
                                          ys[y_indices[yr]]);
            }
            else
                break;
        }
        if(rects.size() > 0)
        {
            constraints.add_rectangle(std::get<0>(rects.back()),
                                  std::get<1>(rects.back()));
        }
    }

    if(output_message)
        std::cout << "#    Generating new particle..." << std::flush;

    // Generate new particle
    // First, clone
    int copy = 0;
    do
    {
        copy = rng.rand_int(options.num_particles);
    }while(options.num_particles > 1 && copy == worst);
    T new_particle = particles[copy];
    double new_x = xs[copy];
    double new_y = ys[copy];

    // Now do Metropolis
    int accepted = 0;
    for(int i=0; i<options.mcmc_steps; ++i)
    {
        // Make proposal

        // Particle
        auto proposal = new_particle;
        double logH = proposal.perturb(rng);

        // Get its scalars
        double proposal_x, proposal_y;
        std::tie(proposal_x, proposal_y) = proposal.get_scalars();

        if(constraints.test(proposal_x, proposal_y) &&
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
        std::cout << accepted << '/' << options.mcmc_steps << '.' << std::endl;
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
    {
        fout.open("output/scalars.csv", std::ios::out);
        fout << "iteration,sampler_id,ln_w_unnormed,f1,f2" << std::endl;
    }

    else
        fout.open("output/scalars.csv", std::ios::out | std::ios::app);

    // Set precision
    fout << std::setprecision(12);

    double ln_w = -double(iteration)/options.num_particles;
    if(sampler_id() != 0)
        ln_w += log(0.5);

    // Output scalars
    fout << iteration << ',';
    fout << sampler_id() << ',';
    fout << ln_w << ',';
    fout << xs[worst] << ',';
    fout << ys[worst] << std::endl;

    // Close the output file
    fout.close();

    // Now output entire particle
    if(iteration % options.thin != 0)
        return;

    if(iteration / options.thin == 1)
        fout.open("output/particles.csv", std::ios::out);
    else
        fout.open("output/particles.csv", std::ios::out | std::ios::app);
    fout << iteration << ',' << sampler_id() << ',';
    fout << particles[worst].render() << std::endl;
    fout.close();
}

} // namespace

#endif

