#ifndef TwinPeaks_Constraints_hpp
#define TwinPeaks_Constraints_hpp

#include <functional>
#include "Ordering.hpp"
#include <set>
#include <tuple>

namespace TwinPeaks
{


bool less_fst(const std::tuple<double, double>& z1,
              const std::tuple<double, double>& z2)
{
    return std::get<0>(z1) < std::get<0>(z2);
}



class Constraints
{
    private:

        // The corners of the rectangles
        std::multiset<std::tuple<double, double>,
                      decltype(&less_fst)> corners;

    public:
        Constraints();

        // Add a new forbidden rectangle
        void add_rectangle(double x, double y);


        // Test a position wrt the forbidden rectangles
        bool test(double x, double y) const;

        // Size
        size_t size() const;

};


/* IMPLEMENTATIONS FOLLOW */

Constraints::Constraints()
:corners(&less_fst)
{

}



void Constraints::add_rectangle(double x, double y)
{
    // First, prune redundant old rectangles
    auto it = corners.begin();
    double _x, _y;
    while(it != corners.end())
    {
        std::tie(_x, _y) = *it;
        if(x >= _x && y >= _y)
            it = corners.erase(it);
        else
            ++it;
    }

    corners.insert(std::tuple<double, double>{x, y});
}


bool Constraints::test(double x, double y) const
{
    // Old inefficient way
    double _x, _y;
    for(auto it=corners.begin(); it != corners.end(); ++it)
    {
        std::tie(_x, _y) = *it;
        if(x < _x && y < _y)
            return false;
    }
    return true;
}


size_t Constraints::size() const
{
    return corners.size();
}

} // namespace

#endif

