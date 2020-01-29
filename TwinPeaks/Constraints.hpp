#ifndef TwinPeaks_Constraints_hpp
#define TwinPeaks_Constraints_hpp

#include <list>

namespace TwinPeaks
{

class Constraints
{
    private:
        std::list<double> forbidden_xs;
        std::list<double> forbidden_ys;

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
    auto it1 = forbidden_xs.begin();
    auto it2 = forbidden_ys.begin();
    while(it1 != forbidden_xs.end() && it2 != forbidden_ys.end())
    {
        if(x >= *it1 && y >= *it2)
        {
            it1 = forbidden_xs.erase(it1);
            it2 = forbidden_ys.erase(it2);
        }

        ++it1;
        ++it2;
    }

    forbidden_xs.push_front(x);
    forbidden_ys.push_front(y);
}


bool Constraints::test(double x, double y) const
{
    auto it1 = forbidden_xs.begin();
    auto it2 = forbidden_ys.begin();
    while(it1 != forbidden_xs.end() && it2 != forbidden_xs.end())
    {
        if(x < *it1 && y < *it2)
            return false;
        ++it1;
        ++it2;
    }
    return true;
}


size_t Constraints::size() const
{
    return forbidden_xs.size();
}

} // namespace

#endif

