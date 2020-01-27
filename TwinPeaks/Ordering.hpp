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

int ranks_to_total_order(int x_rank, int y_rank);

/* Test function for the above */
void test_ranks_to_total_order();


/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::vector<int> compute_ranks(const std::vector<double>& values);

/* IMPLEMENTATIONS FOLLOW */


/*
* Convert ranks (a pair) to integers
* (distance along that path)
*/

int ranks_to_total_order(int x_rank, int y_rank)
{
    int result = 0;
    int tot = x_rank + y_rank;

    // Segments prior to the one we're on
    result += (tot-1)*tot/2;

    // Horizontal or vertical segments
    result += tot;

    // Odd/even
    if(tot % 2 == 0)
        result += x_rank;
    else
        result += y_rank;

    return result;
}

/* Test function for the above */
void test_ranks_to_total_order()
{
    for(int i=1; i<=100; ++i)
    {
        for(int j=1; j<=100; ++j)
        {
            std::cout << i << ' ' << j << ' ' << ranks_to_total_order(i, j);
            std::cout << std::endl; 
        }
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

