#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
    // Create RNG
    RNG rng;

    // Create and run sampler
    Sampler<Example> sampler(1000, rng);
    sampler.run_to_depth(50.0, rng);

    return 0;
}

