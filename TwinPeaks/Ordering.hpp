#ifndef TwinPeaks_Ordering_hpp
#define TwinPeaks_Ordering_hpp

#include <algorithm>
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
    double N = num_particles;
    double X = N - xr - (at_particle)?(0.5):(0.0);
    double Y = N - yr - (at_particle)?(0.5):(0.0);
    return N*N/(X*Y + N*N);
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

