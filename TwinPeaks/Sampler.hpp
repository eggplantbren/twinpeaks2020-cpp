#ifndef TwinPeaks_Sampler_hpp
#define TwinPeaks_Sampler_hpp

#include <iostream>
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

        // The particles
        int num_particles;
        std::vector<T> particles;

        // The particles' scalars and ranks
        std::vector<std::tuple<double, double>> scalars;
        std::vector<std::tuple<int, int>> ranks;

    public:

        // Default constructor disabled
        Sampler() = delete;

        // Constructor where you provide the number of particles
        // and an RNG for particle initialisation
        Sampler(int _num_particles, RNG& rng);

};

/* IMPLEMENTATIONS FOLLOW */

template<typename T>
Sampler<T>::Sampler(int _num_particles, RNG& rng)
:num_particles(_num_particles)
,particles(num_particles)
,scalars(num_particles)
,ranks(num_particles)
{
    // Generate particles from the prior.
    std::cout << "Generating " << num_particles << ' ';
    std::cout << "particles from the prior..." << std::flush;
    for(int i=0; i<num_particles; ++i)
    {
        particles[i].from_prior(rng);
        scalars[i] = particles[i].get_scalars();
    }
    std::cout << "done." << std::endl;
}

} // namespace

#endif

