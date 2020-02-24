#ifndef TwinPeaks_Ordering_hpp
#define TwinPeaks_Ordering_hpp

#include <algorithm>
#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>

namespace TwinPeaks
{


/*
* Convert ranks (a pair) to the target scalar
*/

double Q(int xr, int yr, int num_particles, bool at_particle=true);

/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::tuple<std::vector<int>, std::vector<int>>
        indices_and_ranks(const std::vector<double>& values);

/* IMPLEMENTATIONS FOLLOW */


/*
* Convert ranks (a pair) to the target scalar
*/

double Q(int xr, int yr, int num_particles, bool at_particle)
{
    // Floats
    double xrf = xr;
    double yrf = yr;
    double N = num_particles;
    

    if(at_particle)
    {
        xrf += 0.5;
        yrf += 0.5;
    }

    double tot = xrf + yrf;
    int itot = xr + yr;
    if(at_particle)
        ++itot;

    double result = 0.5*tot*(tot - 1.0) + tot;
    if(itot%2 == 0)
        result += xrf;
    else
        result += yrf;

    return result;
}



/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::tuple<std::vector<int>, std::vector<int>>
        indices_and_ranks(const std::vector<double>& values)
{
    std::vector<int> indices(values.size());
    for(size_t i=0; i<values.size(); ++i)
        indices[i] = i;

    std::sort(indices.begin(), indices.end(), 
                [&](int i, int j)
                { return values[i] < values[j]; });

    std::vector<int> result(values.size());
    for(size_t i=0; i<values.size(); ++i)
        result[indices[i]] = i;

    return {indices, result};
}


} // namespace

#endif

