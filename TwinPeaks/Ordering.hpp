#ifndef TwinPeaks_Ordering_hpp
#define TwinPeaks_Ordering_hpp

namespace TwinPeaks
{

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

} // namespace

#endif

