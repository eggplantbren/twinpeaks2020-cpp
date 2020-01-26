#ifndef TwinPeaks_Ordering_hpp
#define TwinPeaks_Ordering_hpp

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

// Less than or equal to, with a tiebreaker
bool leq(double scalar1, double scalar2, double tb1, double tb2);

/*
* Compute the ranks of scalars[k] with respect
* to all of the scalars in the vector.
*/
std::tuple<int, int>
    compute_rank(int k,
                 const std::vector<std::tuple<double, double>>& scalars,
                 const std::vector<std::tuple<double, double>>& tbs);



/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::vector<std::tuple<int, int>>
    compute_all_ranks(const std::vector<std::tuple<double, double>>& scalars,
                      const std::vector<std::tuple<double, double>>& tbs);

/* IMPLEMENTATIONS FOLLOW */

bool leq(double scalar1, double scalar2, double tb1, double tb2)
{
    if(scalar1 < scalar2)
        return true;

    if(scalar1 == scalar2 && tb1 <= tb2)
        return true;

    return false;
}



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
* Compute the ranks of scalars[k] with respect
* to all of the scalars in the vector.
*/
std::tuple<int, int>
    compute_rank(int k,
                 const std::vector<std::tuple<double, double>>& scalars,
                 const std::vector<std::tuple<double, double>>& tbs)
{
    int x_rank = 0;
    int y_rank = 0;
    for(size_t i=0; i<scalars.size(); ++i)
    {
        if(leq(std::get<0>(scalars[i]), std::get<0>(scalars[k]),
               std::get<0>(tbs[i]),     std::get<0>(tbs[k])))
            ++x_rank;
        if(leq(std::get<1>(scalars[i]), std::get<1>(scalars[k]),
               std::get<1>(tbs[i]),     std::get<1>(tbs[k])))
            ++y_rank;
    }
    return {x_rank, y_rank};
}


/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::vector<std::tuple<int, int>>
    compute_all_ranks(const std::vector<std::tuple<double, double>>& scalars,
                      const std::vector<std::tuple<double, double>>& tbs)
{
    std::vector<std::tuple<int, int>> result(scalars.size());
    for(size_t i=0; i<scalars.size(); ++i)
        result[i] = compute_rank(i, scalars, tbs);
    return result;
}

} // namespace

#endif

