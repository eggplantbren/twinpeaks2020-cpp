#ifndef TwinPeaks_RNG_hpp
#define TwinPeaks_RNG_hpp

#include <cmath>
#include <ctime>
#include <random>

namespace TwinPeaks
{

/*
* An object of this class represents a random number generator state
*/
class RNG
{
    private:

        // 64-bit Mersenne Twister
        std::mt19937_64 twister;

        // For uniform distribution
        std::uniform_real_distribution<double> uniform;

        // For normal distribution
        std::normal_distribution<double> normal;

    public:

        // Constructor that sets seed with the time
        RNG();

        // Constructor that also sets seed
        RNG(unsigned int seed);

        // Set the seed (obviously)
        void set_seed(unsigned int seed);

        // Uniform(0, 1)
        double rand();

        // Normal(0, 1)
        double randn();

        // t(location=0, scale=1, nu=2)
        double randt2();

        // A heavy-tailed distribution for proposals
        double randh();

        // Integer from {0, 1, 2, ..., N-1}
        int rand_int(int N);

        // Access underlying generator
        std::mt19937_64& get_twister();

}; // class RNG


/* IMPLEMENTATION FOLLOWS */

RNG::RNG()
:uniform(0.0, 1.0)
,normal (0.0, 1.0)
{
    set_seed(time(0));
}

RNG::RNG(unsigned int seed)
:uniform(0.0, 1.0)
,normal (0.0, 1.0)
{
    set_seed(seed);
}

void RNG::set_seed(unsigned int seed)
{
    twister.seed(seed);
}

double RNG::rand()
{
    return uniform(twister);
}

double RNG::randn()
{
    return normal(twister);
}

double RNG::randh()
{
    double cauchy = tan(M_PI*(this->rand() - 0.5));
    return pow(10.0, 1.5 - std::abs(cauchy))*this->randn();
}

int RNG::rand_int(int N)
{
    return static_cast<int>(floor(N*this->rand()));
}

std::mt19937_64& RNG::get_twister()
{
    return twister;
}

} // namespace

#endif

