#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
    // Create RNG
    RNG rng;

    // Create and run sampler
    Sampler<Example> sampler(RunOptions(50.0), rng);
    sampler.run(rng);

    return 0;
}

