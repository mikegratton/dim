#include <cassert>
#include <iostream>
#include <dim/si.hpp> // 1
using namespace dim::si; // 2
using namespace dim::si::literal; // 3

Length compute_distance(Time t, Speed v) { return t*v; }

// uncomment to see compile error
// Length compute_distance_bad(Time t, Speed v) { return 0.5*t*t*v; } 

int main(int argc, char** argv)
{
    Time my_time = 3.0_s; // 4
    Speed my_speed = 2.0 * foot/second; // 5
    Length my_distance = compute_distance(my_time, my_speed);
    std::cout << "In " << my_time << ", an object traveling at speed " << my_speed 
              << " travels " << my_distance << "\n";
    assert(sizeof(my_distance) == sizeof(double));
    return 0;
}
