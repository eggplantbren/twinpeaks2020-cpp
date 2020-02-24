#include <iostream>
#include <TwinPeaks/Harder.hpp>
#include <TwinPeaks/Potts.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
//    test_d(3);
    // Create RNG
    RNG rng;

    // Create and run sampler
    Sampler<Harder> sampler(RunOptions(1000, 1000, 10, 120.0), rng);
    sampler.run(rng);

    return 0;
}

