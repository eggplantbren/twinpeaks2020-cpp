#include <iostream>
#include <TwinPeaks/Example.hpp>
#include <TwinPeaks/Sampler.hpp>

using namespace TwinPeaks;

int main()
{
    RNG rng;
    Sampler<Example> sampler(1000, rng);

    for(int i=0; i<100000; ++i)
        sampler.advance(rng);

    return 0;
}

