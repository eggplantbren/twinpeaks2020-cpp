#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Potts.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
    // Create RNG
    RNG rng;

    // Create and run sampler
    Sampler<Potts> sampler(RunOptions(100, 1000, 100, 1000.0), rng);
    sampler.run(rng);

    return 0;
}

