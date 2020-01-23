#ifndef TwinPeaks_Potts_hpp
#define TwinPeaks_Potts_hpp

#include <iomanip>
#include "Misc.hpp"
#include "RNG.hpp"
#include <sstream>
#include <string>
#include <tuple>


namespace TwinPeaks
{

/*
* Potts model.
*/
class Potts
{
    private:

        // Potts parameters
        static constexpr int size = 32;
        static constexpr int num_colors = 5;

        // Cell values
        std::vector<std::vector<int>> x;

        // So we can just update on the fly
        int score;
        int score2;

        void compute_score();
        void compute_scalars();


    public:

        // Constructor
        Potts();

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

Potts::Potts()
:x(size, std::vector<int>(size))
{

}


void Potts::from_prior(RNG& rng)
{
    for(int i=0; i<size; ++i)
        for(int j=0; j<size; ++j)
            x[i][j] = rng.rand_int(num_colors);

    score = 0;
    score2 = 0;

    // Coordinates of neighbouring cells
    std::vector<int> ii(4); std::vector<int> jj(4);

    for(int i=0; i<size; ++i)
    {
        for(int j=0; j<size; ++j)
        {
            for(int k=0; k<4; ++k)
            {
                ii[k] = i;
                jj[k] = j;
            }

            // Down, up, right, left
            ii[0] = TwinPeaks::mod(i + 1, size);
            ii[1] = TwinPeaks::mod(i - 1, size);
            jj[2] = TwinPeaks::mod(j + 1, size);
            jj[3] = TwinPeaks::mod(j - 1, size);

            for(int k=0; k<4; ++k)
                if(x[i][j] == x[ii[k]][jj[k]])
                    score++;
            score2 += (i - j)*x[i][j];
        }
    }
}

double Potts::perturb(RNG& rng)
{
    int reps = 1;
    if(rng.rand() <= 0.5)
        reps += 1 + rng.rand_int(9);

    // Which cell is being perturbed
    int i, j;

    // Coordinates of neighbouring cells
    std::vector<int> ii(4); std::vector<int> jj(4);
    for(int z=0; z<reps; ++z)
    {
        i = rng.rand_int(size);
        j = rng.rand_int(size);

        for(int k=0; k<4; ++k)
        {
            ii[k] = i;
            jj[k] = j;
        }

        // Down, up, right, left
        ii[0] = TwinPeaks::mod(i + 1, size);
        ii[1] = TwinPeaks::mod(i - 1, size);
        jj[2] = TwinPeaks::mod(j + 1, size);
        jj[3] = TwinPeaks::mod(j - 1, size);

        // Calculate negative part of delta score
        for(int k=0; k<4; ++k)
            if(x[i][j] == x[ii[k]][jj[k]])
                score--;
        score2 -= (i - j)*x[i][j];

        // Perturb the cell
        x[i][j] = rng.rand_int(num_colors);

        // Calculate positive part of delta score
        for(int k=0; k<4; ++k)
            if(x[i][j] == x[ii[k]][jj[k]])
                ++score;
        score2 += (i - j)*x[i][j];
    }

    return 0.0;
}

std::tuple<double, double> Potts::get_scalars() const
{
    return {score, score2};
}

std::string Potts::render() const
{
    std::stringstream ss;
    ss << std::setprecision(12);
    for(int i=0; i<size; ++i)
    {
        for(int j=0; j<size; ++j)
        {
            ss << x[i][j];
            if(i == size-1 && j == size-1)
                ss << "";
            else
                ss << ',';
        }
    }
    return ss.str();
}



} // namespace

#endif

