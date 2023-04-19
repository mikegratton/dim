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

template<>
void print_unit<dim::si::system>(char* buf, dynamic_unit const& unit, bool spaceit) {
    if (spaceit) {
        buf += sprintf(buf, "_");
    }
    for (std::size_t i=0; i<unit.size(); i++) {
        detail::print_helper<::dim::si::system>(&buf, unit[i], i, spaceit);
    }
}

using namespace si;

template<> format_map<Temperature> const& get_default_format<Temperature>() {
    static const format_map<Temperature> s_known {
        {"F", formatter<Temperature>("F", 5./9.*kelvin, 255.37*kelvin)},
        {"R", formatter<Temperature>("R", 5./9.*kelvin)},
        {"C", formatter<Temperature>("C", 1.*kelvin, 273.15*kelvin)}

    };
    return s_known;
}

template<>
format_map<Length> const& get_default_format<Length>() {
    static const format_map<Length> s_known {
        {"in",   formatter<Length>("in", inch) },
        {"inch", formatter<Length>("inch", inch)},
        {"ft",   formatter<Length>("ft", foot)},
        {"foot", formatter<Length>("foot", foot)},
        {"yd",   formatter<Length>("yd", yard)},
        {"yard", formatter<Length>("yard", yard)},
        {"mi",   formatter<Length>("mi", mile)},
        {"mile", formatter<Length>("mile", mile)},
        {"nmi",  formatter<Length>("nmi", nautical_mile)},
        {"nautical_mile", formatter<Length>("nautical_mile", nautical_mile)}
    };
    return s_known;
}

template<>
format_map<Time> const& get_default_format<Time>() {
    static const format_map<Time> s_known {
        {"min",    formatter<Time>("min", minute)},
        {"h",      formatter<Time>("h", hour)},
        {"hr",     formatter<Time>("hr", hour)},
        {"minute", formatter<Time>("minute", minute)},
        {"hour",   formatter<Time>("hour", hour)}
    };
    return s_known;
}

template<>
format_map<Mass> const& get_default_format<Mass>() {
    static const format_map<Mass> s_known {
        {"oz",         formatter<Mass>("oz", pound_mass / 16.0)},
        {"lb",         formatter<Mass>("lb", pound_mass)},
        {"lbm",        formatter<Mass>("lbm", pound_mass)},
        {"slug",       formatter<Mass>("slug", slug)},
        {"ounce",      formatter<Mass>("ounce", pound_mass / 16.0)},
        {"pound",      formatter<Mass>("pound", pound_mass)},
        {"pound_mass", formatter<Mass>("pound_mass", pound_mass)}
    };
    return s_known;
}

template<>
format_map<Angle> const& get_default_format<Angle>() {
    static const format_map<Angle> s_known {
        {"radian",      formatter<Angle>("radian", radian)},
        {"deg",         formatter<Angle>("deg", degree)},
        {"mil",         formatter<Angle>("mil", 1e-3 * radian)},
        {"mrad",        formatter<Angle>("mrad", 1e-3 * radian)},
        {"milliradian", formatter<Angle>("milliradian", 1e-3 * radian)},
        {"turn",        formatter<Angle>("turn", 2.0 * M_PI * radian)},
        {"'",           formatter<Angle>("'", degree / 60.0)},
        {"min",         formatter<Angle>("min", degree / 60.0)},
        {"minute",      formatter<Angle>("minute", degree / 60.0)},
        {"\"",          formatter<Angle>("\"", degree / 3600.0)},
        {"s",           formatter<Angle>("s", degree / 3600.0)},
        {"second",      formatter<Angle>("second", degree / 3600.0)}
    };
    return s_known;
}

template<>
format_map<SolidAngle> const& get_default_format<SolidAngle>() {
    static const format_map<SolidAngle> s_known {
        {"steradian", formatter<SolidAngle>("steradian", steradian)},
        {"sp",        formatter<SolidAngle>("sp", 4.0 * M_PI * steradian)},
        {"spat",      formatter<SolidAngle>("spat", 4.0 * M_PI * steradian)}
    };
    return s_known;
}


template<>
format_map<Force> const& get_default_format<Force>() {
    static const format_map<Force> s_known {
        {"dyn",         formatter<Force>("dyn", dyne)},
        {"lb",          formatter<Force>("lb", pound_force)},
        {"lbf",         formatter<Force>("lbf", pound_force)},
        {"pound",       formatter<Force>("pound", pound_force)},
        {"pound_force", formatter<Force>("pound_force", pound_force)}
    };
    return s_known;
}

template<>
format_map<Pressure> const& get_default_format<Pressure>() {
    static const format_map<Pressure> s_known {
        {"lbf/in^2",               formatter<Pressure>("lbf/in^2", pound_force / inch / inch)},
        {"lbf_in^-2",              formatter<Pressure>("lbf_in^-2", pound_force / inch / inch)},
        {"lb/in^2",                formatter<Pressure>("lb/in^2", pound_force / inch / inch)},
        {"lb_in^-2",               formatter<Pressure>("lb_in^-2", pound_force / inch / inch)},
        {"pounds_per_inch2",       formatter<Pressure>("pounds_per_inch2", pound_force / inch / inch)},
        {"pounds_per_square_inch", formatter<Pressure>("pounds_per_square_inch", pound_force / inch / inch)},
        {"torr",                   formatter<Pressure>("torr", atmosphere / 760.)},
        {"atm",                    formatter<Pressure>("atm", atmosphere)},
        {"atmosphere",             formatter<Pressure>("atmosphere", atmosphere)}
    };
    return s_known;
}

template<>
format_map<Energy> const& get_default_format<Energy>() {
    static const format_map<Energy> s_known {
        {"kW_hr",       formatter<Energy>("kW_hr", 1e3*watt*hour)},
        {"kW*hr",       formatter<Energy>("kW_hr", 1e3*watt*hour)},
        {"kWhr",        formatter<Energy>("kW_hr", 1e3*watt*hour)},
        {"erg",         formatter<Energy>("erg", erg)},
        {"foot_pound",  formatter<Energy>("foot_pound", foot * pound_force)},
        {"ft_lb",       formatter<Energy>("ft_lb", foot * pound_force)},
        {"ft_lbf",      formatter<Energy>("ft_lbf", foot * pound_force)},
        {"BTU",         formatter<Energy>("BTU", 1055.06 * joule)}
    };
    return s_known;
}

template<>
format_map<Power> const& get_default_format<Power>() {
    static const format_map<Power> s_known {
        {"hp",          formatter<Power>("hp", 33e3 * foot* pound_force / minute)}
    };
    return s_known;
}

template<>
format_map<Area> const& get_default_format<Area>() {
    static const format_map<Area> s_known {
        {"acre",  formatter<Area>("acre", acre)},
        {"sq_mi", formatter<Area>("sq_mi", mile * mile)},
        {"mi^2",  formatter<Area>("mi^2", mile * mile)},
        {"mile2", formatter<Area>("mile2", mile * mile)},
        {"sq_ft", formatter<Area>("sq_ft", foot * foot)},
        {"ft^2",  formatter<Area>("ft^2", foot * foot)},
        {"foot2", formatter<Area>("foot2", foot * foot)}
    };
    return s_known;
}

template<>
format_map<Volume> const& get_default_format<Volume>() {
    static const format_map<Volume> s_known {
        {"cc",         formatter<Volume>("cc", 1e-6 * meter3)},
        {"gal",        formatter<Volume>("gal", gallon)},
        {"gallon",     formatter<Volume>("gallon", gallon)},
        {"acre_ft",    formatter<Volume>("acre_ft", acre * foot)},
        {"acre_foot",  formatter<Volume>("acre_foot", acre * foot)},
        {"cu_ft",      formatter<Volume>("cu_ft", foot* foot * foot)},
        {"ft^3",       formatter<Volume>("ft^3", foot* foot * foot)},
        {"cubic_foot", formatter<Volume>("cubic_foot", foot* foot * foot)},
        {"cu_in",      formatter<Volume>("cu_in", inch* inch * inch)},
        {"in^3",       formatter<Volume>("in^3", inch* inch * inch)},
        {"cubic_inch", formatter<Volume>("cubic_inch", inch* inch * inch)},
        {"cu_yd",      formatter<Volume>("cu_yd", yard* yard * yard)},
        {"yd^3",       formatter<Volume>("yd^3", yard* yard * yard)},
        {"cubic_yard", formatter<Volume>("cubic_yard", yard* yard * yard)},
    };
    return s_known;
}

template<>
format_map<FlowRate> const& get_default_format<FlowRate>() {
    static const format_map<FlowRate> s_known {
        {"gal/s",         formatter<FlowRate>("gal/s", gallon / second)},
        {"gal/min",       formatter<FlowRate>("gal/min", gallon / minute)},
        {"meter3/second", formatter<FlowRate>("meter3/second", meter3 / second)},
        {"liter/second",  formatter<FlowRate>("liter/second", liter / second)},
        {"gallon/second", formatter<FlowRate>("gallon/second", gallon / second)},
        {"gallon/minute", formatter<FlowRate>("gallon/minute", gallon / minute)},
    };
    return s_known;
}
template<>
format_map<Speed> const& get_default_format<Speed>() {
    static const format_map<Speed> s_known {
        {"mps",             formatter<Speed>("mps", meter/second)},
        {"kph",             formatter<Speed>("kph", 1e3*meter/hour)},
        {"mph",             formatter<Speed>("mph", mile / hour)},
        {"miles_per_hour",  formatter<Speed>("miles_per_hour", mile / hour)},
        {"knot",            formatter<Speed>("knot", knot)},
        {"kn",              formatter<Speed>("kn", knot)},
        {"kt",              formatter<Speed>("kt", knot)},
        {"ft/s",            formatter<Speed>("ft/s", foot / second)},
        {"feet_per_second", formatter<Speed>("feet_per_second", foot / second)}
    };
    return s_known;
}
template<>
format_map<Acceleration> const& get_default_format<Acceleration>() {
    static const format_map<Acceleration> s_known {
        {"ft/s^2", formatter<Acceleration>("ft/s^2", foot / second / second)},
        {"feet_per_second2", formatter<Acceleration>("feet_per_second2", foot / second / second)}
    };
    return s_known;
}
template<>
format_map<AngularRate> const& get_default_format<AngularRate>() {
    static const format_map<AngularRate> s_known {
        {"deg/s",              formatter<AngularRate>("deg/s", degree / second)},
        {"degrees_per_second", formatter<AngularRate>("degrees_per_second", degree / second)},
        {"radians_per_second", formatter<AngularRate>("radians_per_second", radian / second)}
    };
    return s_known;
}
template<>
format_map<AngularAcceleration> const& get_default_format<AngularAcceleration>() {
    static const format_map<AngularAcceleration> s_known {
        {"deg/s^2",             formatter<AngularAcceleration>("deg/s^2", degree / second / second)},
        {"degrees_per_second2", formatter<AngularAcceleration>("degrees_per_second2", degree / second / second)},
        {"radians_per_second2", formatter<AngularAcceleration>("radians_per_second2", radian / second / second)}
    };
    return s_known;
}
template<>
format_map<Torque> const& get_default_format<Torque>() {
    static const format_map<Torque> s_known {
        {"ft*lbf",     formatter<Torque>("ft*lbf", foot* pound_force/radian)},
        {"ft_lbf",     formatter<Torque>("ft_lbf", foot* pound_force/radian)},
        {"ft*lb",      formatter<Torque>("ft*lb", foot* pound_force/radian)},
        {"ft_lb",      formatter<Torque>("ft_lb", foot* pound_force/radian)},
        {"foot_pound", formatter<Torque>("foot_pound", foot* pound_force/radian)}
    };
    return s_known;
}

}

// Use the Bison/Flex parser. The includes for these are fairly involved,
// so we put them down here to avoid polluting anything else.

// SERIOUSLY??? Flex needs you to tell it this. Include order also matters
#define YYSTYPE QUANTITYSTYPE
#include "quantity.tab.hpp"
#include "quantity.lex.hpp"

namespace dim {
namespace detail {
template<>
::dim::si::dynamic_quantity parse_standard_rep<si::system, double>(const char* unit_str, int nend) {
    if (unit_str == nullptr || unit_str[0] <= 31 || unit_str[0] >= 126) {
        return ::dim::si::dynamic_quantity::bad_quantity();
    }
    ::dim::si::dynamic_quantity result; // Our return value
    
    // Construct the flex scanner
    yyscan_t scanner;
    if (quantitylex_init(&scanner)) {
        return ::dim::si::dynamic_quantity::bad_quantity();
    }
    
    // Make a buffer with the text to scan
    YY_BUFFER_STATE buf;
    if (nend > 0) {
        buf = quantity_scan_bytes(unit_str, nend, scanner);
    } else {
        buf = quantity_scan_string(unit_str, scanner);
    }
    
    // Scan it
    int r = quantityparse(scanner, &result); 
    if (r != 0) {        
        result = ::dim::si::dynamic_quantity::bad_quantity();
    }
    
    // Clean up
    quantity_delete_buffer(buf, scanner);
    quantitylex_destroy(scanner);
    return result;
}
}
}
