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

double d(int ix, int iy, bool corner=false);

/* Test function for the above */
void test_d();


/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::vector<int> compute_ranks(const std::vector<double>& values);

/* IMPLEMENTATIONS FOLLOW */


/*
* Convert ranks (a pair) to integers
* (distance along that path)
*/

double d(int ix, int iy, bool corner)
{
    if(corner)
        return 0.5*(d(ix+1, iy) + d(ix, iy+1));

    /****** ELSE ******/

    double result = 0.0;
    int tot = ix + iy;

    // Segments prior to the one we're on
    result += 0.5*tot*(tot - 1);

    // Horizontal or vertical segments
    result += tot;

    // Odd/even
    if(tot % 2 == 0)
        result += ix;
    else
        result += iy;

// Alternative that pushes away from the diagonals, didn't seem to help
//    // Off diagonal part
//    int off_diag_dist;
//    if(iy == ix)
//        off_diag_dist = 0;
//    if(tot % 2 == 0)
//    {
//        // Even tot
//        off_diag_dist += std::abs(iy - ix);
//        if(iy < ix)
//            --off_diag_dist;
//    }
//    else
//    {
//        // Odd tot
//        off_diag_dist += std::abs(iy - ix);
//        if(iy < ix)
//            --off_diag_dist;
//    }
//    result += off_diag_dist;


    return result;
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
std::vector<int> compute_ranks(const std::vector<double>& values)
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

    return result;
}


} // namespace

#endif

