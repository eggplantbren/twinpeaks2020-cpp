#ifndef TwinPeaks_Constraints_hpp
#define TwinPeaks_Constraints_hpp

#include <boost/container/flat_set.hpp>
#include <functional>
#include "Ordering.hpp"
#include <set>
#include <tuple>

namespace TwinPeaks
{


// Represent a point with < based on x
// but then y can break a tie [in reverse order].
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
        if(x == other.x && y > other.y) // Note the >
            return true;
        return false;
    }  
};



class Constraints
{
    private:

        // The corners of the rectangles
        boost::container::flat_set<Point,
                                        std::less<>> corners;

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
    // First, prune redundant old rectangles
    // Can't erase very well from a boost flat_set so copy over to a vector
    // temporarily.
    std::vector<Point> temp;
    for(auto it=corners.begin(); it != corners.end(); ++it)
    {
        if(x < it->x || y < it->y)
            temp.push_back(*it);
    }

    // Recreate the set after pruning
    corners.clear();
    for(auto c: temp)
        corners.insert(c);

    // Add the new point.
    corners.emplace(Point{x, y});
}


bool Constraints::test(double x, double y) const
{
    // Old inefficient way
    for(auto it=corners.begin(); it != corners.end(); ++it)
    {
        if(x < it->x && y < it->y)
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

