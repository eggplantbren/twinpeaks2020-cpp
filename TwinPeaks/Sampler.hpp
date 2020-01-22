#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include <fstream>
#include <iomanip>
#include <iostream>
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
    write_output();

    // TODO



    ++iteration;
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

}

} // namespace

#endif

