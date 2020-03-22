#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Sampler.hpp>

using Model = TwinPeaks::Example;
using Sampler = TwinPeaks::Sampler<Model>;

int main()
{
    // Create RNG
    TwinPeaks::RNG rng;

    // Create and run sampler
    Sampler sampler(TwinPeaks::RunOptions(1000, 1000, 10, 120.0), rng);
    sampler.run(rng);

    return 0;
}

