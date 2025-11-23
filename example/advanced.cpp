#include <cassert>
#include <iostream>
#include <dim/si.hpp>
namespace si = ::dim::si;

// Making a quantity alias
using Pitch = decltype(si::meter/si::radian);

// Making our own literal formatters
namespace dim {
namespace si {
namespace literal {
inline constexpr Angle operator "" _deg(long double x) { return static_cast<double>(x)*degree; } \
inline constexpr Angle operator "" _deg(unsigned long long x) { return static_cast<double>(x)*degree; } 
}
}
}

using namespace dim::si::literal;

int main(int argc, char** argv)
{
    si::system::install_facet();
    si::Torque T = 3_N * 2_m / 1.2_rad;    
    std::cout << "Torque is " << T << "\n";    
    // Let's format as inches and degrees
    auto* facet = si::make_default_facet();
    facet->output_formatter(si::formatter("in", si::inch));
    facet->output_formatter(si::formatter("deg", si::degree));
    facet->output_formatter(si::formatter("in/deg", si::inch/si::degree));
    std::locale enhanced_locale(std::locale{}, facet);
    std::locale::global(enhanced_locale);    
    
    Pitch screw = 2.0 * si::inch / 30.0_deg;    
    std::cout << "Screw pitch is " << screw << "\n";    
    return 0;
}
