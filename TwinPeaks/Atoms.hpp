#ifndef TwinPeaks_Atoms_hpp
#define TwinPeaks_Atoms_hpp

#include <iomanip>
#include "Misc.hpp"
#include "RNG.hpp"
#include <sstream>
#include <string>
#include <tuple>

namespace TwinPeaks
{

/*
* A simple Atoms model.
*/
class Atoms
{
    private:

        // Number of atoms
        static constexpr int N = 20;

        // 2D positions
        std::vector<double> xs, ys;

        // Particle-particle potential energy terms
        std::vector<std::vector<double>> terms;
        void compute_terms();
        void update_terms(int k);

    public:

        // Constructor
        Atoms();

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

Atoms::Atoms()
:xs(N)
,ys(N)
,terms(N, std::vector<double>(N, 0.0))
{

}

void Atoms::update_terms(int k)
{
    double rsq;
    for(int i=k+1; i<N; ++i)
    {
        rsq = pow(xs[i] - xs[k], 2) + pow(ys[i] - ys[k], 2);
        terms[i][k] = 0.5*rsq;
    }
    for(int j=k+1; j<N; ++j)
    {
        rsq = pow(xs[k] - xs[j], 2) + pow(ys[k] - ys[j], 2);
        terms[k][j] = 0.5*rsq;
    }

}

void Atoms::compute_terms()
{
    for(int k=0; k<N; ++k)
        update_terms(k);
}

void Atoms::from_prior(RNG& rng)
{
    for(int i=0; i<N; ++i)
    {
        xs[i] = rng.rand();
        ys[i] = rng.rand();
    }

    compute_terms();
}

double Atoms::perturb(RNG& rng)
{
    int reps = 1;
//    if(rng.rand() <= 0.5)
//        reps = (int)pow((double)3, rng.rand());

    for(int rep=0; rep<reps; ++rep)
    {
        int k = rng.rand_int(N);
        double& coord = (rng.rand() <= 0.5)?(xs[k]):(ys[k]);
        coord += rng.randh();
        wrap(coord, 0.0, 1.0);
        update_terms(k);
    }

    return 0;
}

std::tuple<double, double> Atoms::get_scalars() const
{
    double f1 = 0.0;
    double f2 = 0.0;

    for(int i=0; i<N; ++i)
    {
        for(int j=0; j<i; ++j)
            f1 += -terms[i][j];
        f2 += -ys[i];
    }

    return {f1, f2};
}

std::string Atoms::render() const
{
    std::stringstream ss;
    ss << std::setprecision(12);

    for(int i=0; i<N; ++i)
        ss << xs[i] << ',';
    for(int i=0; i<N; ++i)
    {
        ss << ys[i];
        if(i < (N-1))
            ss << ',';
    }
    return ss.str();
}

} // namespace

#endif

