#include "dim/si/definition.hpp"
#include "dim/si/si_facet.hpp"
#include "dim/system_creation_helper.hpp"
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
DIM_MAKE_LITERAL(deg, degree)
DIM_MAKE_LITERAL(in, inch)
DIM_MAKE_LITERAL(lbf, pound_force)
}
}
}

// We must put these user-defined literals into the global namespace
// to use them
using namespace dim::si::literal;

int main(int argc, char** argv)
{
    si::system::install_facet();
    si::Torque T = 3_lbf * 2_in / 1.2_deg;    
    std::cout << "Default SI facet format: ";
    std::cout << "Torque is " << T << "\n";

    // Let's format as inches and degrees
    auto* facet = si::make_default_facet();
    facet->output_formatter(si::formatter("in", si::inch));
    facet->output_formatter(si::formatter("deg", si::degree));
    facet->output_formatter(si::formatter("in/deg", si::inch/si::degree));
    // Note: this also imbues all of the iostream members (in particular, cout for this example)
    si::install_facet(facet);
        
    Pitch screw = 2.0_in / 30.0_deg;  
    std::cout << "Custom format: ";  
    std::cout << "Screw pitch is " << screw << "\n";    
    return 0;
}
