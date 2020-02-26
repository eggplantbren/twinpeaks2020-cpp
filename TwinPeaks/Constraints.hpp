#ifndef TwinPeaks_Constraints_hpp
#define TwinPeaks_Constraints_hpp

#include <algorithm>
#include "Ordering.hpp"
#include <vector>

namespace TwinPeaks
{


// Represent a point with < based on x
// but then y can break a tie [reverse order for y!]
struct Point
{
    double x;
    double y;

    Point(std::initializer_list<double> l)
    {
        auto it = l.begin();
        x = *it;
        ++it;
        y = *it;
    }

    bool operator < (const Point& other) const
    {
        if(x < other.x)
            return true;
        if(x == other.x && y > other.y) // y goes in reverse
            return true;
        return false;
    }  
};



class Constraints
{
    private:

        // The corners of the rectangles
        // Using sorted vector.
        // See http://lafstern.org/matt/col1.pdf
        std::vector<Point> corners;

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
{

}



void Constraints::add_rectangle(double x, double y)
{
    // If new rectangle is redundant, don't do anything
    if(!test(x, y))
        return;

    // Prune rectangles made redundant bu the new one
    for(auto it=corners.begin(); it != corners.end();)
    {
        if(x >= it->x && y >= it->y)
            it = corners.erase(it);
        else
            ++it;
    }

    // Add the new rectangle (in the right place)
    auto it = std::lower_bound(corners.begin(), corners.end(), Point{x, y});
    corners.insert(it, Point{x, y});
}


bool Constraints::test(double x, double y) const
{
    // The rectangle just after (x, y) in terms of x, if there is one.
    auto it = std::lower_bound(corners.begin(), corners.end(), Point{x, y});
    if(it == corners.end())
        return true;
    return y >= it->y;
}


size_t Constraints::size() const
{
    return corners.size();
}

} // namespace

#endif

