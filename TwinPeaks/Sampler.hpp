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
        std::vector<std::tuple<double, double>> scalars;
        std::vector<std::tuple<double, double>> tbs;
        std::vector<std::tuple<int, int>> ranks;

        // Distance along space filling curve
        std::vector<int> dists;
        std::vector<double> dist_tiebreakers;

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
,scalars(run_options.num_particles)
,tbs(run_options.num_particles)
,ranks(run_options.num_particles)
,dists(run_options.num_particles)
,dist_tiebreakers(run_options.num_particles)
{
    // Generate particles from the prior.
    std::cout << "Generating " << run_options.num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;
    for(int i=0; i<run_options.num_particles; ++i)
    {
        particles[i].from_prior(rng);
        dist_tiebreakers[i] = rng.rand();
        scalars[i] = particles[i].get_scalars();
        tbs[i] = {rng.rand(), rng.rand()};
    }
    compute_orderings();
    std::cout << "done." << std::endl;

    // Save a record of the run options
    run_options.save();
}

template<typename T>
void Sampler<T>::compute_orderings()
{
    // Compute all ranks of all particles
    ranks = compute_all_ranks(scalars, tbs);

    // Map ranks to total order
    std::vector<double> dists_plus_tiebreakers(run_options.num_particles);
    for(int i=0; i<run_options.num_particles; ++i)
    {
        int x_rank, y_rank;
        std::tie(x_rank, y_rank) = ranks[i];
        dists[i] = ranks_to_total_order(x_rank, y_rank);
        dists_plus_tiebreakers[i] = dists[i] + dist_tiebreakers[i];
    }

    // Find worst particle
    worst = 0;
    for(int i=1; i<run_options.num_particles; ++i)
        if(dists_plus_tiebreakers[i] < dists_plus_tiebreakers[worst])
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
    // Progress message
    std::cout << std::setprecision(12);
    std::cout << "Iteration " << iteration << ". ";
    std::cout << "Depth ~= " << (double)iteration/run_options.num_particles << " nats.";
    std::cout << std::endl;

    // Save worst particle
    std::cout << "    Saving worst particle..." << std::flush;
    write_output();
    std::cout << "done. ";
    std::cout << "Scalars = (" << std::get<0>(scalars[worst]) << ", ";
    std::cout << std::get<1>(scalars[worst]) << ")." << std::endl;
    if(last_iteration)
        return;

    std::cout << "    Generating replacement particle..." << std::flush;
    // Generate new particle
    // First, clone
    int copy = 0;
    do
    {
        copy = rng.rand_int(run_options.num_particles);
    }while(run_options.num_particles > 1 && copy == worst);
    T new_particle = particles[copy];
    auto new_particle_scalars = scalars[copy];
    auto new_particle_tbs = tbs[copy];
    double new_dist_tiebreaker = dist_tiebreakers[copy];

    // Now do Metropolis
    int accepted = 0;
    for(int i=0; i<run_options.mcmc_steps; ++i)
    {
        // Make proposal

        // Particle
        auto proposal = new_particle;
        double logH = proposal.perturb(rng);

        // Get its scalars
        auto proposal_scalars = proposal.get_scalars();

        // Propose tiebreakers
        auto proposal_tbs = new_particle_tbs;
        double u1, u2;
        std::tie(u1, u2) = proposal_tbs;
        double& u = (rng.rand() < 0.5)?(u1):(u2);
        u += rng.randh();
        wrap(u, 0.0, 1.0);
        proposal_tbs = {u1, u2};

        // Propose dist tiebreaker
        double proposal_dist_tiebreaker = new_dist_tiebreaker + rng.randh();
        wrap(proposal_dist_tiebreaker, 0.0, 1.0);

        // Compute ranks
        int xr = 0;
        int yr = 0;
        for(int j=0; j<run_options.num_particles; ++j)
        {
            if(leq(std::get<0>(scalars[j]), std::get<0>(proposal_scalars),
                   std::get<0>(tbs[j]),     std::get<0>(proposal_tbs)))
                ++xr;
            if(leq(std::get<1>(scalars[j]), std::get<1>(proposal_scalars),
                   std::get<1>(tbs[j]),     std::get<1>(proposal_tbs)))
                ++yr;
        }

        int proposal_dist = ranks_to_total_order(xr, yr);
        if((proposal_dist + proposal_dist_tiebreaker >=
            dists[worst] + dist_tiebreakers[worst]) && 
           (rng.rand() <= exp(logH)))
        {
            new_particle = proposal;
            new_particle_scalars = proposal_scalars;
            new_particle_tbs = proposal_tbs;
            new_dist_tiebreaker = proposal_dist_tiebreaker;
            ++accepted;
        }

    }

    // Insert new particle and recompute stuff
    particles[worst] = new_particle;
    scalars[worst] = new_particle_scalars;
    tbs[worst] = new_particle_tbs;

    // Is this necessary? Could I just re-generate them all?
    dist_tiebreakers[worst] = new_dist_tiebreaker;
    std::cout << "done. ";
    std::cout << "Metropolis acceptance rate = ";
    std::cout << accepted << '/' << run_options.mcmc_steps << '.' << std::endl;

    // Compute new orderings.
    std::cout << "    Computing new orderings..." << std::flush;
    compute_orderings();
    std::cout << "done." << std::endl;

    // Increment iteration counter
    ++iteration;
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
    fout << std::get<0>(scalars[worst]) << ',';
    fout << std::get<1>(scalars[worst]) << std::endl;

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

