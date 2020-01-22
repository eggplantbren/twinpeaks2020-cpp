#ifndef TwinPeaks_Example_hpp
#define TwinPeaks_Example_hpp

#include "RNG.hpp"
#include <tuple>

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

        // Return scalars
        std::tuple<double, double> get_scalars() const;

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

std::tuple<double, double> Example::get_scalars() const
{
    double f1 = 0.0;
    double f2 = 0.0;

    // Parameters of the two peaks
    static constexpr double w = 0.01;
    static constexpr double tau = 1.0/(w*w);

    for(double x: xs)
    {
        f1 += -0.5*tau*pow(x - 0.45, 2);
        f2 += -0.5*tau*pow(x - 0.55, 2);
    }

    return {f1, f2};
}

} // namespace

#endif

