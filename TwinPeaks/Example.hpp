#ifndef TwinPeaks_Example_hpp
#define TwinPeaks_Example_hpp

#include "RNG.hpp"

namespace TwinPeaks
{

/*
* A simple example model.
*/
class Example
{
    private:
        static constexpr int N = 10; // Dimensionality
        std::vector<double> xs;

    public:
        // Constructor must generate from prior
        Example();
        void from_prior(RNG& rng);

};

/* IMPLEMENTATIONS FOLLOW */

Example::Example()
:xs(N)
{

}

void Example::from_prior(RNG& rng)
{
    for(int i=0; i<N; ++i)
        xs[i] = rng.rand();
}

} // namespace

#endif

