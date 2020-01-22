#ifndef TwinPeaks_Ordering_hpp
#define TwinPeaks_Ordering_hpp

#include <tuple>
#include <vector>

namespace TwinPeaks
{


/*
* Convert ranks (a pair) to integers
* (distance along that path)
*/

int ranks_to_total_order(int x_rank, int y_rank);


/*
* Compute the ranks of scalars[k] with respect
* to all of the scalars in the vector.
*/
std::tuple<int, int>
    compute_rank(int k,
                const std::vector<std::tuple<double, double>>& scalars);



/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::vector<std::tuple<int, int>>
    compute_all_ranks(const std::vector<std::tuple<double, double>>& scalars);


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
    for(int i=0; i<tot; ++i)
        result += i;

    // Horizontal or vertical segments
    result += tot;

    // Odd/even
    if(tot % 2 == 0)
        result += x_rank;
    else
        result += y_rank;

    return result;
}


/*
* Compute the ranks of scalars[k] with respect
* to all of the scalars in the vector.
*/
std::tuple<int, int>
    compute_rank(int k,
                const std::vector<std::tuple<double, double>>& scalars)
{
    int x_rank = 0;
    int y_rank = 0;
    for(size_t i=0; i<scalars.size(); ++i)
    {
        if(std::get<0>(scalars[i]) <= std::get<0>(scalars[k]))
            ++x_rank;
        if(std::get<1>(scalars[i]) <= std::get<1>(scalars[k]))
            ++y_rank;
    }
    return {x_rank, y_rank};
}


/*
* Compute ranks of all scalars with respect to the whole collection.
*/
std::vector<std::tuple<int, int>>
    compute_all_ranks(const std::vector<std::tuple<double, double>>& scalars)
{
    std::vector<std::tuple<int, int>> result(scalars.size());
    for(size_t i=0; i<scalars.size(); ++i)
        result[i] = compute_rank(i, scalars);
    return result;
}


} // namespace

#endif

