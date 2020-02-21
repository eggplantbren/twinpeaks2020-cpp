#ifndef TwinPeaks_Constraints_hpp
#define TwinPeaks_Constraints_hpp

#include <cstdlib>
#include <iostream>

// SOCI headers
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

namespace TwinPeaks
{

class Constraints
{
    private:
        sqlite::database db;

    public:
        Constraints();

        // Add a new forbidden rectangle
        void add_rectangle(double x, double y);

        // Test a position wrt the forbidden rectangles
        bool test(double x, double y);

        // Size
        int size();
};


/* IMPLEMENTATIONS FOLLOW */

Constraints::Constraints()
:db(":memory:")
{
    db << "CREATE TABLE rectangles\
            (id INTEGER PRIMARY KEY,\
             xs REAL,\
             ys REAL);";

    db << "CREATE INDEX x_idx ON rectangles (xs);";
    db << "CREATE INDEX y_idx ON rectangles (ys);";
    db << "CREATE INDEX xy_idx ON rectangles (xs, ys);";
    db << "CREATE INDEX yx_idx ON rectangles (ys, xs);";
}




void Constraints::add_rectangle(double x, double y)
{
    db << "BEGIN;";
    db << "DELETE FROM rectangles WHERE xs < ? AND ys < ?;" << x << y;
    db << "INSERT INTO rectangles (xs, ys) VALUES (?, ?);"  << x << y;
    db << "COMMIT;";
}


bool Constraints::test(double x, double y)
{
    int count;
    db << "SELECT COUNT(*) FROM\
                (SELECT * FROM rectangles WHERE xs > ? AND ys > ? LIMIT 1);"
       << x << y >> count;
    return (count == 0);
}


int Constraints::size()
{
    int count;
    db << "SELECT COUNT(*) FROM rectangles;" >> count;
    return count;
}

} // namespace

#endif

