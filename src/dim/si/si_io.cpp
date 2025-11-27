#include "si_io.hpp"

namespace dim {

using namespace si;

// clang-format off

template<> si::input_format_map const& get_default_format<Temperature>() {
    static const si::input_format_map s_known {
        {"F", 5./9.*kelvin, 255.37*kelvin},
        {"R", 5./9.*kelvin},
        {"C", 1.*kelvin, 273.15*kelvin}
    };
    return s_known;
}

template<>
si::input_format_map const& get_default_format<Length>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<Time>() {
    static const si::input_format_map s_known {
        {"min", minute},
        {"h", hour},
        {"hr", hour},
        {"minute", minute},
        {"hour", hour}
    };
    return s_known;
}

template<>
si::input_format_map const& get_default_format<Mass>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<Angle>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<SolidAngle>() {
    static const si::input_format_map s_known {
        {"steradian", steradian},
        {"sp", 4.0 * M_PI * steradian},
        {"spat", 4.0 * M_PI * steradian}
    };
    return s_known;
}


template<>
si::input_format_map const& get_default_format<Force>() {
    static const si::input_format_map s_known {
        {"dyn", dyne},
        {"lb", pound_force},
        {"lbf", pound_force},
        {"pound", pound_force},
        {"pound_force", pound_force}
    };
    return s_known;
}

template<>
si::input_format_map const& get_default_format<Pressure>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<Energy>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<Power>() {
    static const si::input_format_map s_known {
        {"hp", 33e3 * foot* pound_force / minute}
    };
    return s_known;
}

template<>
si::input_format_map const& get_default_format<Area>() {
    static const si::input_format_map s_known {
        {"acre", acre},
        {"sq_mi", mile * mile},
        {"mi^2", mile * mile},
        {"mile2", mile * mile},
        {"sq_ft", foot * foot},
        {"ft^2", foot * foot},
        {"foot2", foot * foot},
        {"a", 1e2*meter2},
        {"ha", 1e4*meter2}
    };
    return s_known;
}

template<>
si::input_format_map const& get_default_format<Volume>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<FlowRate>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<Speed>() {
    static const si::input_format_map s_known {
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
si::input_format_map const& get_default_format<Acceleration>() {
    static const si::input_format_map s_known {
        {"ft/s^2", foot / second / second},
        {"feet_per_second2", foot / second / second}
    };
    return s_known;
}
template<>
si::input_format_map const& get_default_format<AngularRate>() {
    static const si::input_format_map s_known {
        {"deg/s", degree / second},
        {"degrees_per_second", degree / second},
        {"radians_per_second", radian / second}
    };
    return s_known;
}
template<>
si::input_format_map const& get_default_format<AngularAcceleration>() {
    static const si::input_format_map s_known {
        {"deg/s^2", degree / second / second},
        {"degrees_per_second2", degree / second / second},
        {"radians_per_second2", radian / second / second}
    };
    return s_known;
}
template<>
si::input_format_map const& get_default_format<Torque>() {
    static const si::input_format_map s_known {
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

// Use the Bison/Flex parser.
#include "quantity_parser_driver.hpp"

namespace dim {
namespace detail {

template <>
::dim::si::dynamic_quantity parse_standard_rep<double, si::system>(const char* unit_str, int nend)
{
    dim::si::detail::quantity_parser_driver driver;
    driver.parse(unit_str, nend);
    return driver.result;
}
}  // namespace detail
}  // namespace dim
