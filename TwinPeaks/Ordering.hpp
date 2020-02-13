#ifndef TwinPeaks_Ordering_hpp
#define TwinPeaks_Ordering_hpp

#include <algorithm>
#include <iostream>
#include <tuple>
#include <vector>

namespace TwinPeaks
{


/*
* Convert ranks (a pair) to integers
* (distance along that path)
*/

double d  (int ix, int iy, bool corner=false);
double dtb(int ix, int iy, bool corner=false);

/* Test function for the above */
void test_d();

/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::tuple<std::vector<int>, std::vector<int>>
        indices_and_ranks(const std::vector<double>& values);

/* IMPLEMENTATIONS FOLLOW */


/*
* Convert ranks (a pair) to integers
* (distance along that path)
*/

double d(int ix, int iy, bool corner)
{
    if(corner)
        return 0.5*(d(ix, iy) + d(ix+1, iy+1));
    return ix+iy;
}

double dtb(int ix, int iy, bool corner)
{
    if(corner)
        return 0.5*(dtb(ix, iy) + dtb(ix+1, iy+1));
    if(ix+iy % 2 == 0)
        return ix;
    return iy;
}

/* Test function for the above */
void test_d(int num_particles)
{
    for(int iy=num_particles; iy>=0; --iy)
    {
        for(int ix=0; ix<=num_particles; ++ix)
            std::cout << d(ix, iy) << ' ';
        std::cout << std::endl; 
    }
    std::cout << std::endl;
    for(int iy=num_particles-1; iy>=0; --iy)
    {
        for(int ix=0; ix<num_particles; ++ix)
            std::cout << d(ix, iy, true) << ' ';
        std::cout << std::endl; 
    }
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

