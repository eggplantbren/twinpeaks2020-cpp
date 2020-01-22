#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
    RNG rng;
    Sampler<Example> sampler(100, rng);

    return 0;
}

