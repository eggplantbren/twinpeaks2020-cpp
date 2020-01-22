#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include <fstream>
#include <iomanip>
#include <iostream>
#include "Misc.hpp"
#include "Ordering.hpp"
#include <ostream>
#include "RNG.hpp"
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

        // Thinning for full particle output
        static constexpr int thin = 10;

        // Iteration counter
        int iteration;

        // The particles
        int num_particles;
        std::vector<T> particles;

        // The particles' scalars and ranks
        std::vector<std::tuple<double, double>> scalars;
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

    public:

        // Default constructor disabled
        Sampler() = delete;

        // Constructor where you provide the number of particles
        // and an RNG for particle initialisation
        Sampler(int _num_particles, RNG& rng);

        // Do one NS iteration
        void advance(RNG& rng);
};

/* IMPLEMENTATIONS FOLLOW */

template<typename T>
Sampler<T>::Sampler(int _num_particles, RNG& rng)
:iteration(1)
,num_particles(_num_particles)
,particles(num_particles)
,scalars(num_particles)
,ranks(num_particles)
,dists(num_particles)
,dist_tiebreakers(num_particles)
{
    // Generate particles from the prior.
    std::cout << "Generating " << num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;
    for(int i=0; i<num_particles; ++i)
    {
        particles[i].from_prior(rng);
        dist_tiebreakers[i] = rng.rand();
        scalars[i] = particles[i].get_scalars();
    }
    compute_orderings();
    std::cout << "done." << std::endl;
}

template<typename T>
void Sampler<T>::compute_orderings()
{
    // Compute all ranks of all particles
    ranks = compute_all_ranks(scalars);

    // Map ranks to total order
    std::vector<double> dists_plus_tiebreakers(num_particles);
    for(int i=0; i<num_particles; ++i)
    {
        int x_rank, y_rank;
        std::tie(x_rank, y_rank) = ranks[i];
        dists[i] = ranks_to_total_order(x_rank, y_rank);
        dists_plus_tiebreakers[i] = dists[i] + dist_tiebreakers[i];
    }

    // Find worst particle
    worst = 0;
    for(int i=1; i<num_particles; ++i)
        if(dists_plus_tiebreakers[i] < dists_plus_tiebreakers[worst])
            worst = i;
}

template<typename T>
void Sampler<T>::advance(RNG& rng)
{
    std::cout << "Iteration " << iteration << '.' << std::endl;

    // Save worst particle
    std::cout << "    Saving worst particle..." << std::flush;
    write_output();
    std::cout << "done." << std::endl;

    std::cout << "    Generating replacement particle..." << std::flush;
    // Generate new particle
    // First, clone
    int copy = 0;
    do
    {
        copy = rng.rand_int(num_particles);
    }while(num_particles > 1 && copy == worst);
    T new_particle = particles[copy];
    auto new_particle_scalars = scalars[copy];
    double new_dist_tiebreaker = dist_tiebreakers[copy];

    // Now do Metropolis
    static constexpr int mcmc_steps = 1000;
    int accepted = 0;
    for(int i=0; i<mcmc_steps; ++i)
    {
        // Make proposal (for both particle and tiebreaker value)
        auto proposal = new_particle;
        double logH = proposal.perturb(rng);
        double proposal_dist_tiebreaker = new_dist_tiebreaker + rng.randh();
        wrap(proposal_dist_tiebreaker, 0.0, 1.0);
        auto proposal_scalars = proposal.get_scalars();

        // Compute ranks
        int xr = 0;
        int yr = 0;
        for(int j=0; j<num_particles; ++j)
        {
            if(std::get<0>(scalars[j]) <= std::get<0>(proposal_scalars))
                ++xr;
            if(std::get<1>(scalars[j]) <= std::get<1>(proposal_scalars))
                ++yr;
        }

        int proposal_dist = ranks_to_total_order(xr, yr);
        if((proposal_dist + proposal_dist_tiebreaker >=
            dists[worst] + dist_tiebreakers[worst]) && 
           (rng.rand() <= exp(logH)))
        {
            new_particle = proposal;
            new_particle_scalars = proposal_scalars;
            new_dist_tiebreaker = proposal_dist_tiebreaker;
            ++accepted;
        }

    }

    // Insert new particle and recompute stuff
    particles[worst] = new_particle;
    scalars[worst] = new_particle_scalars;

    // Is this necessary? Could I just re-generate them all?
    dist_tiebreakers[worst] = new_dist_tiebreaker;
    std::cout << "done. ";
    std::cout << "Metropolis acceptance rate = ";
    std::cout << accepted << '/' << mcmc_steps << '.' << std::endl;

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
    if(iteration % thin != 0)
        return;

    if(iteration / thin == 1)
        fout.open("output/particles.csv", std::ios::out);
    else
        fout.open("output/particles.csv", std::ios::out | std::ios::app);
    fout << iteration << ',';
    fout << particles[worst].render() << std::endl;
    fout.close();
}

} // namespace

#endif

