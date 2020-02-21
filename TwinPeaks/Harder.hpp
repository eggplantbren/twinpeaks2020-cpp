#ifndef TwinPeaks_Harder_hpp
#define TwinPeaks_Harder_hpp

#include <iomanip>
#include "Misc.hpp"
#include "RNG.hpp"
#include <sstream>
#include <string>
#include <tuple>

namespace TwinPeaks
{

/*
* A simple Harder model.
*/
class Harder
{
    private:
        static constexpr int N = 20; // Dimensionality
        std::vector<double> xs;

    public:

        // Constructor
        Harder();

        // Generate from prior
        void from_prior(RNG& rng);

        // Metropolis proposal in-place, return log(hastings) for prior sampling
        double perturb(RNG& rng);

        // Return scalars
        std::tuple<double, double> get_scalars() const;

        // Render as string
        std::string render() const;

};

/* IMPLEMENTATIONS FOLLOW */

Harder::Harder()
:xs(N)
{

}

void Harder::from_prior(RNG& rng)
{
    for(int i=0; i<N; ++i)
        xs[i] = rng.rand();
}

double Harder::perturb(RNG& rng)
{
    int k = rng.rand_int(N);
    xs[k] += rng.randh();
    wrap(xs[k], 0.0, 1.0);
    return 0;
}

std::tuple<double, double> Harder::get_scalars() const
{
    double f1 = 0.0;
    double f2 = 0.0;

    // Parameters of the two peaks
    static constexpr double w = 0.01;
    static constexpr double tau = 1.0/(w*w);

    for(int i=0; i<N; ++i)
    {
        f1 += -0.5*tau*pow(xs[i] - 0.5, 2);
        f2 += -xs[i]/w;
    }

    return {f1, f2};
}

std::string Harder::render() const
{
    std::stringstream ss;
    ss << std::setprecision(12);
    for(int i=0; i<N; ++i)
    {
        ss << xs[i];
        if(i < (N-1))
            ss << ',';
    }
    return ss.str();
}

} // namespace

#endif

