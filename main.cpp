#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Potts.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
//    test_d(3);
    // Create RNG
    RNG rng;

    // Create and run sampler
    Sampler<Example> sampler(RunOptions(1000, 1000, 10, 1.0), rng);
    sampler.run(rng);

    return 0;
}

