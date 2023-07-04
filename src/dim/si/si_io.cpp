#include "si_io.hpp"

/*
#include <iostream>
#include <string>

std::ostream& operator<<(std::ostream& os, ::dim::dynamic_quantity<double, ::dim::si::system> const& dq) {
    os << dq.value << " [ ";
    for (int i=0; i<dq.unit.size(); i++) {
        os << (int) dq.unit[i] << " ";
    }
    os << "]";
    return os;
}
*/

namespace dim {

template <>
void print_unit<dim::si::system>(char* buf, dynamic_unit const& unit, bool spaceit)
{
    if (spaceit) { buf += sprintf(buf, "_"); }
    for (std::size_t i = 0; i < unit.size(); i++) {
        detail::print_helper<::dim::si::system>(&buf, unit[i], i, spaceit);
    }
}

using namespace si;

// clang-format off

template<> format_map<Temperature> const& get_default_format<Temperature>() {
    static const format_map<Temperature> s_known {
        {"F", 5./9.*kelvin, 255.37*kelvin},
        {"R", 5./9.*kelvin},
        {"C", 1.*kelvin, 273.15*kelvin}
    };
    return s_known;
}

template<>
format_map<Length> const& get_default_format<Length>() {
    static const format_map<Length> s_known {
        {"in", inch },
        {"inch", inch},
        {"ft", foot},
        {"foot", foot},
        {"yd", yard},
        {"yard", yard},
        {"mi", mile},
        {"mile", mile},
        {"nmi", nautical_mile},
        {"nautical_mile", nautical_mile}
    };
    return s_known;
}

template<>
format_map<Time> const& get_default_format<Time>() {
    static const format_map<Time> s_known {
        {"min", minute},
        {"h", hour},
        {"hr", hour},
        {"minute", minute},
        {"hour", hour}
    };
    return s_known;
}

template<>
format_map<Mass> const& get_default_format<Mass>() {
    static const format_map<Mass> s_known {
        {"oz", pound_mass / 16.0},
        {"lb", pound_mass},
        {"lbm", pound_mass},
        {"slug", slug},
        {"ounce", pound_mass / 16.0},
        {"pound", pound_mass},
        {"pound_mass", pound_mass}
    };
    return s_known;
}

template<>
format_map<Angle> const& get_default_format<Angle>() {
    static const format_map<Angle> s_known {
        {"radian", radian},
        {"deg", degree},
        {"mil", 1e-3 * radian},
        {"mrad", 1e-3 * radian},
        {"milliradian", 1e-3 * radian},
        {"turn", 2.0 * M_PI * radian},
        {"'", degree / 60.0},
        {"min", degree / 60.0},
        {"minute", degree / 60.0},
        {"\"", degree / 3600.0},
        {"s", degree / 3600.0},
        {"second", degree / 3600.0}
    };
    return s_known;
}

template<>
format_map<SolidAngle> const& get_default_format<SolidAngle>() {
    static const format_map<SolidAngle> s_known {
        {"steradian", steradian},
        {"sp", 4.0 * M_PI * steradian},
        {"spat", 4.0 * M_PI * steradian}
    };
    return s_known;
}


template<>
format_map<Force> const& get_default_format<Force>() {
    static const format_map<Force> s_known {
        {"dyn", dyne},
        {"lb", pound_force},
        {"lbf", pound_force},
        {"pound", pound_force},
        {"pound_force", pound_force}
    };
    return s_known;
}

template<>
format_map<Pressure> const& get_default_format<Pressure>() {
    static const format_map<Pressure> s_known {
        {"lbf/in^2", pound_force / inch / inch},
        {"lbf_in^-2", pound_force / inch / inch},
        {"lb/in^2", pound_force / inch / inch},
        {"lb_in^-2", pound_force / inch / inch},
        {"pounds_per_inch2", pound_force / inch / inch},
        {"pounds_per_square_inch", pound_force / inch / inch},
        {"torr", atmosphere / 760.},
        {"atm", atmosphere},
        {"atmosphere", atmosphere}
    };
    return s_known;
}

template<>
format_map<Energy> const& get_default_format<Energy>() {
    static const format_map<Energy> s_known {
        {"kW_hr", 1e3*watt*hour},
        {"kW_hr", 1e3*watt*hour},
        {"kW_hr", 1e3*watt*hour},
        {"erg", erg},
        {"foot_pound", foot * pound_force},
        {"ft_lb", foot * pound_force},
        {"ft_lbf", foot * pound_force},
        {"BTU", 1055.06 * joule}
    };
    return s_known;
}

template<>
format_map<Power> const& get_default_format<Power>() {
    static const format_map<Power> s_known {
        {"hp", 33e3 * foot* pound_force / minute}
    };
    return s_known;
}

template<>
format_map<Area> const& get_default_format<Area>() {
    static const format_map<Area> s_known {
        {"acre", acre},
        {"sq_mi", mile * mile},
        {"mi^2", mile * mile},
        {"mile2", mile * mile},
        {"sq_ft", foot * foot},
        {"ft^2", foot * foot},
        {"foot2", foot * foot}
    };
    return s_known;
}

template<>
format_map<Volume> const& get_default_format<Volume>() {
    static const format_map<Volume> s_known {
        {"cc", 1e-6 * meter3},
        {"gal", gallon},
        {"gallon", gallon},
        {"acre_ft", acre * foot},
        {"acre_foot", acre * foot},
        {"cu_ft", foot* foot * foot},
        {"ft^3", foot* foot * foot},
        {"cubic_foot", foot* foot * foot},
        {"cu_in", inch* inch * inch},
        {"in^3", inch* inch * inch},
        {"cubic_inch", inch* inch * inch},
        {"cu_yd", yard* yard * yard},
        {"yd^3", yard* yard * yard},
        {"cubic_yard", yard* yard * yard},
    };
    return s_known;
}

template<>
format_map<FlowRate> const& get_default_format<FlowRate>() {
    static const format_map<FlowRate> s_known {
        {"gal/s", gallon / second},
        {"gal/min", gallon / minute},
        {"meter3/second", meter3 / second},
        {"liter/second", liter / second},
        {"gallon/second", gallon / second},
        {"gallon/minute", gallon / minute},
    };
    return s_known;
}
template<>
format_map<Speed> const& get_default_format<Speed>() {
    static const format_map<Speed> s_known {
        {"mps", meter/second},
        {"kph", 1e3*meter/hour},
        {"mph", mile / hour},
        {"miles_per_hour", mile / hour},
        {"knot", knot},
        {"kn", knot},
        {"kt", knot},
        {"ft/s", foot / second},
        {"feet_per_second", foot / second}
    };
    return s_known;
}
template<>
format_map<Acceleration> const& get_default_format<Acceleration>() {
    static const format_map<Acceleration> s_known {
        {"ft/s^2", foot / second / second},
        {"feet_per_second2", foot / second / second}
    };
    return s_known;
}
template<>
format_map<AngularRate> const& get_default_format<AngularRate>() {
    static const format_map<AngularRate> s_known {
        {"deg/s", degree / second},
        {"degrees_per_second", degree / second},
        {"radians_per_second", radian / second}
    };
    return s_known;
}
template<>
format_map<AngularAcceleration> const& get_default_format<AngularAcceleration>() {
    static const format_map<AngularAcceleration> s_known {
        {"deg/s^2", degree / second / second},
        {"degrees_per_second2", degree / second / second},
        {"radians_per_second2", radian / second / second}
    };
    return s_known;
}
template<>
format_map<Torque> const& get_default_format<Torque>() {
    static const format_map<Torque> s_known {
        {"ft*lbf", foot* pound_force/radian},
        {"ft_lbf", foot* pound_force/radian},
        {"ft*lb", foot* pound_force/radian},
        {"ft_lb", foot* pound_force/radian},
        {"foot_pound", foot* pound_force/radian}
    };
    return s_known;
}
// clang-format on
}  // namespace dim

// Use the Bison/Flex parser. The includes for these are fairly involved,
// so we put them down here to avoid polluting anything else.

// clang-format off
// SERIOUSLY??? Flex needs you to tell it this. Include order also matters
#define YYSTYPE QUANTITYSTYPE
#include "quantity.tab.hpp"
#include "quantity.lex.hpp"

namespace dim {
namespace detail {
template<>
::dim::si::dynamic_quantity parse_standard_rep<si::system, double>(const char* unit_str, int nend) {
    // Construct the flex scanner for re-use throughout the program   
    static thread_local yyscan_t s_scanner;
    static thread_local bool s_init = false;
    if (s_init == false) {
        quantitylex_init(&s_scanner);
        s_init = true;
    }
    
    ::dim::si::dynamic_quantity result; // Our return value
    
    // Make a buffer with the text to scan
    YY_BUFFER_STATE buf;    
    if (nend == -1) {
        buf = quantity_scan_string(unit_str, s_scanner);
    } else {
        buf = quantity_scan_bytes(unit_str, nend, s_scanner);
    }
    
    // Scan it
    int r = quantityparse(s_scanner, &result); 
    if (r != 0) {        
        result = ::dim::si::dynamic_quantity::bad_quantity();
    }
    
    // Clean up
    quantity_delete_buffer(buf, s_scanner);    
    return result;
}
}
}
