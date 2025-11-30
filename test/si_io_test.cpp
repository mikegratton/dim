#include "dim/dynamic_quantity.hpp"
#include "dim/format_map.hpp"
#include "dim/io_detail.hpp"
#include "dim/iostream.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/literal.hpp"
#include "dim/si/si_io.hpp"
#include "doctest.h"
#include <cstdlib>
#include <iostream>

using namespace dim::si;
using namespace dim;

void doParseCheck(char const* text, double value, si::dynamic_unit const& unit)
{
    char* endptr;
    double v = strtod(text, &endptr);
    if (detail::isseparator(*endptr)) {
        endptr++;
    }
    using namespace dim::detail;
    auto q = v * dim::detail::parse_standard_rep<double, dim::si::system>(endptr, endptr + strlen(endptr));

    CHECK_MESSAGE(q.is_bad() == false, "Failed to parse ", std::string(text), ", got v = ", v,
                  ", unit = ", std::string(endptr));
    CHECK(q.value() == doctest::Approx(value));
    CHECK(q.unit() == unit);
}

void doParseFailCheck(char const* text)
{
    using namespace dim::detail;
    auto q = dim::detail::parse_standard_rep<double, dim::si::system>(text, text + strlen(text));
    CHECK(q.is_bad() == true);
}

TEST_CASE("SiParse")
{
    // Dimensionless
    doParseCheck("0.1", 0.1, {0, 0, 0, 0, 0, 0, 0, 0});

    // Base units
    doParseCheck("0_m", 0.0, {1, 0, 0, 0, 0, 0, 0, 0});
    doParseCheck("0_s", 0.0, {0, 1, 0, 0, 0, 0, 0, 0});
    doParseCheck("0_kg", 0.0, {0, 0, 1, 0, 0, 0, 0, 0});
    doParseCheck("0_rad", 0.0, {0, 0, 0, 1, 0, 0, 0, 0});
    doParseCheck("0_K", 0.0, {0, 0, 0, 0, 1, 0, 0, 0});
    doParseCheck("0_mol", 0.0, {0, 0, 0, 0, 0, 1, 0, 0});
    doParseCheck("0_A", 0.0, {0, 0, 0, 0, 0, 0, 1, 0});
    doParseCheck("0_cd", 0.0, {0, 0, 0, 0, 0, 0, 0, 1});

    // Operators, exponents, and parens
    doParseCheck("1_m^2*s^-1", 1.0, {2, -1, 0, 0, 0, 0, 0, 0});

    doParseCheck("1_m^2*s^(-1)", 1.0, {2, -1, 0, 0, 0, 0, 0, 0});

    doParseCheck("1_(m^2*s^-1)", 1.0, {2, -1, 0, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2 s^-1", 1.0, {2, -1, 0, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2/s", 1.0, {2, -1, 0, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2/s^2", 1.0, {2, -2, 0, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2/s^-2", 1.0, {2, 2, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_m^((2))*s^-1", 1.0, {2, -1, 0, 0, 0, 0, 0, 0});
    doParseFailCheck("1_m^kg*s^-1");

    // Other units recognized
    doParseCheck("1_Hz", 1e0, {0, -1, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_sr", 1e0, {0, 0, 0, 2, 0, 0, 0, 0});
    doParseCheck("1_N", 1e0, {1, -2, 1, 0, 0, 0, 0, 0});

    doParseCheck("1_Pa", 1e0, {-1, -2, 1, 0, 0, 0, 0, 0});

    doParseCheck("1_J", 1e0, {2, -2, 1, 0, 0, 0, 0, 0});
    doParseCheck("1_W", 1e0, {2, -3, 1, 0, 0, 0, 0, 0});
    doParseCheck("1_C", 1e0, {0, 1, 0, 0, 0, 0, 1, 0});
    doParseCheck("1_V", 1e0, {2, -3, 1, 0, 0, 0, -1, 0});
    doParseCheck("1_F", 1e0, {-2, 4, -1, 0, 0, 0, 2, 0});
    doParseCheck("1_R", 1e0, {2, -3, 1, 0, 0, 0, -2, 0});
    doParseCheck("1_S", 1e0, {-2, 3, -1, 0, 0, 0, 2, 0});
    doParseCheck("1_Wb", 1e0, {2, -2, 1, 0, 0, 0, -1, 0});
    doParseCheck("1_T", 1e0, {0, -2, 1, 0, 0, 0, -1, 0});
    doParseCheck("1_H", 1e0, {2, -2, 1, 0, 0, 0, -2, 0});
    doParseCheck("1_Im", 1e0, {0, 0, 0, 2, 0, 0, 0, 1});
    doParseCheck("1_Ix", 1e0, {-2, 0, 0, 2, 0, 0, 0, 1});
    doParseCheck("1_Bq", 1e0, {0, -1, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_Gy", 1e0, {2, -2, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_Sv", 1e0, {2, -2, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_kat", 1e0, {0, -1, 0, 0, 0, 1, 0, 0});
    doParseCheck("1_L", 1e-3, {3, 0, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_eV", 1.60218e-19, {2, -2, 1, 0, 0, 0, 0, 0});
    // doParseCheck("1_a", 1e2, {2, 0, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_bar", 1e5, {-1, -2, 1, 0, 0, 0, 0, 0});

    // UTF-8 support
    doParseCheck("1_μm", 1e-6, {1, 0, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_Ω", 1.0, si::dynamic_unit(si::Resistance::unit{}));
}

TEST_CASE("print_unit")
{
    char buf[256];
    char* end = buf + sizeof(buf);

    print_unit(buf, end, index(si::meter));
    CHECK(std::string("m") == buf);

    print_unit(buf, end, index(si::second));
    CHECK(std::string("s") == buf);

    print_unit(buf, end, index(si::kilogram));
    CHECK(std::string("kg") == buf);

    print_unit(buf, end, index(si::radian));
    CHECK(std::string("rad") == buf);

    print_unit(buf, end, index(si::kelvin));
    CHECK(std::string("K") == buf);

    print_unit(buf, end, index(si::mole));
    CHECK(std::string("mol") == buf);

    print_unit(buf, end, index(si::ampere));
    CHECK(std::string("A") == buf);

    print_unit(buf, end, index(si::candela));
    CHECK(std::string("cd") == buf);

    // Compound
    print_unit(buf, end, index<si::Speed>());
    CHECK(std::string("m_s^-1") == buf);

    auto q = si::meter / pow<2>(si::second) / pow<3>(si::radian);
    print_unit(buf, end, index(q));
    CHECK(std::string("rad^-3_m_s^-2") == buf);

    print_unit(buf, end, pow(index(si::meter), 5));
    CHECK(std::string("m^5") == buf);
}

TEST_CASE("si.format.temperature")
{
    si::install_facet();
    si::Temperature temp;

    // Note: for affine transforms, we need to check two values to be sure the
    // transform is right
    CHECK(parse_quantity(temp, {1.0, "F"}));
    CHECK(temp == si::fahrenheit(1.0));
    CHECK(parse_quantity(temp, {0.0, "F"}));
    CHECK(temp == si::fahrenheit(0.0));
    CHECK(parse_quantity(temp, {1.0, "℉"}));
    CHECK(temp == si::fahrenheit(1.0));
    CHECK(parse_quantity(temp, {0.0, "℉"}));
    CHECK(temp == si::fahrenheit(0.0));
    CHECK(parse_quantity(temp, {1.0, "C"}));
    CHECK(temp == si::celsius(1.0));
    CHECK(parse_quantity(temp, {0.0, "C"}));
    CHECK(temp == si::celsius(0.0));
    CHECK(parse_quantity(temp, {1.0, "℃"}));
    CHECK(temp == si::celsius(1.0));
    CHECK(parse_quantity(temp, {0.0, "℃"}));
    CHECK(temp == si::celsius(0.0));

    // These are linear transforms
    CHECK(parse_quantity(temp, {1.0, "R"}));
    CHECK(temp == 1.0 * si::rankine);
    CHECK(parse_quantity(temp, {1.0, "K"}));
    CHECK(temp == 1.0 * si::kelvin);
    CHECK(parse_quantity(temp, {1.0, "K"}));
    CHECK(temp == 1.0 * si::kelvin);
}

TEST_CASE("si.format.length")
{
    si::Length x;
    CHECK(parse_quantity(x, {1.0, "metre"}));
    CHECK(x == si::meter);
    CHECK(parse_quantity(x, {1.0, "in"}));
    CHECK(x == si::inch);
    CHECK(parse_quantity(x, {1.0, "inch"}));
    CHECK(x == si::inch);
    CHECK(parse_quantity(x, {1.0, "ft"}));
    CHECK(x == si::foot);
    CHECK(parse_quantity(x, {1.0, "foot"}));
    CHECK(x == si::foot);
    CHECK(parse_quantity(x, {1.0, "yd"}));
    CHECK(x == si::yard);
    CHECK(parse_quantity(x, {1.0, "yard"}));
    CHECK(x == si::yard);
    CHECK(parse_quantity(x, {1.0, "mi"}));
    CHECK(x == si::mile);
    CHECK(parse_quantity(x, {1.0, "mile"}));
    CHECK(x == si::mile);
    CHECK(parse_quantity(x, {1.0, "nmi"}));
    CHECK(x == si::nautical_mile);
    CHECK(parse_quantity(x, {1.0, "nautical_mile"}));
    CHECK(x == si::nautical_mile);
    CHECK(parse_quantity(x, {1.0, "Å"}));
    CHECK(x == 1e-10 * si::meter);
}

TEST_CASE("si.format.time")
{
    si::Time x;
    CHECK(parse_quantity(x, {1.0, "min"}));
    CHECK(x == si::minute);
    CHECK(parse_quantity(x, {1.0, "minute"}));
    CHECK(x == si::minute);
    CHECK(parse_quantity(x, {1.0, "h"}));
    CHECK(x == si::hour);
    CHECK(parse_quantity(x, {1.0, "hr"}));
    CHECK(x == si::hour);
    CHECK(parse_quantity(x, {1.0, "hour"}));
    CHECK(x == si::hour);
}

TEST_CASE("si.format.mass")
{
    si::Mass x;
    CHECK(parse_quantity(x, {1.0, "oz"}));
    CHECK(x == si::pound_mass / 16.0);
    CHECK(parse_quantity(x, {1.0, "ounce"}));
    CHECK(x == si::pound_mass / 16.0);
    CHECK(parse_quantity(x, {1.0, "lb"}));
    CHECK(x == si::pound_mass);
    CHECK(parse_quantity(x, {1.0, "lbm"}));
    CHECK(x == si::pound_mass);
    CHECK(parse_quantity(x, {1.0, "pound"}));
    CHECK(x == si::pound_mass);
    CHECK(parse_quantity(x, {1.0, "pound_mass"}));
    CHECK(x == si::pound_mass);
    CHECK(parse_quantity(x, {1.0, "slug"}));
    CHECK(x == si::slug);
}

TEST_CASE("si.format.angle")
{
    si::Angle x;
    CHECK(parse_quantity(x, {1.0, "radian"}));
    CHECK(x == si::radian);
    CHECK(parse_quantity(x, {1.0, "deg"}));
    CHECK(x == si::degree);
    CHECK(parse_quantity(x, {1.0, "°"}));
    CHECK(x == si::degree);
    CHECK(parse_quantity(x, {1.0, "second"}));
    CHECK(x == si::degree/ 3600.0);
    CHECK(parse_quantity(x, {1.0, "sec"}));
    CHECK(x == si::degree/ 3600.0);
    CHECK(parse_quantity(x, {1.0, "s"}));
    CHECK(x == si::degree/ 3600.0);
    CHECK(parse_quantity(x, {1.0, "\""}));
    CHECK(x == si::degree/ 3600.0);
    CHECK(parse_quantity(x, {1.0, "minute"}));
    CHECK(x == si::degree/ 60.0);
    CHECK(parse_quantity(x, {1.0, "min"}));
    CHECK(x == si::degree/ 60.0);
    CHECK(parse_quantity(x, {1.0, "m"}));
    CHECK(x == si::degree/ 60.0);
    CHECK(parse_quantity(x, {1.0, "'"}));
    CHECK(x == si::degree/ 60.0);
    CHECK(parse_quantity(x, {1.0, "milliradian"}));
    CHECK(x == 1e-3 * si::radian);
    CHECK(parse_quantity(x, {1.0, "mil"}));
    CHECK(x == 1e-3 * si::radian);
    CHECK(parse_quantity(x, {1.0, "mrad"}));
    CHECK(x == 1e-3 * si::radian);
    CHECK(parse_quantity(x, {1.0, "turn"}));
    CHECK(x == 2.0 * M_PI * si::radian);
    CHECK(parse_quantity(x, {1.0, "tr"}));
    CHECK(x == 2.0 * M_PI * si::radian);
    CHECK(parse_quantity(x, {1.0, "rev"}));
    CHECK(x == 2.0 * M_PI * si::radian);
    CHECK(parse_quantity(x, {1.0, "cyc"}));
    CHECK(x == 2.0 * M_PI * si::radian);
}

TEST_CASE("si.format.solid_angle")
{
    si::SolidAngle x;
    CHECK(parse_quantity( x, {1.0, "steradian"}));
    CHECK(x == si::steradian);
    CHECK(parse_quantity( x, {1.0, "sr"}));
    CHECK(x == si::steradian);
    CHECK(parse_quantity( x, {1.0, "sp"}));
    CHECK(x == 4.0 * M_PI * si::steradian);
    CHECK(parse_quantity( x, {1.0, "spat"}));
    CHECK(x == 4.0 * M_PI * si::steradian);
}

TEST_CASE("si.format.force")
{
    si::Force x;
    CHECK(parse_quantity(x, {1.0, "dyn"}));
    CHECK(x == si::dyne);
    CHECK(parse_quantity(x, {1.0, "dyne"}));
    CHECK(x == si::dyne);
    CHECK(parse_quantity(x, {1.0, "lb"}));
    CHECK(x == si::pound_force);
    CHECK(parse_quantity(x, {1.0, "lbf"}));
    CHECK(x == si::pound_force);
    CHECK(parse_quantity(x, {1.0, "pound_force"}));
    CHECK(x == si::pound_force);
}

TEST_CASE("si.format.pressure")
{
    si::Pressure x;
    CHECK(parse_quantity(x, {1.0, "lbf/in^2"}));
    CHECK(x == pound_force/inch/inch);
    CHECK(parse_quantity(x, {1.0, "lbf_in^-2"}));
    CHECK(x == pound_force/inch/inch);
    CHECK(parse_quantity(x, {1.0, "lb/in^2"}));
    CHECK(x == pound_force/inch/inch);
    CHECK(parse_quantity(x, {1.0, "lb_in^-2"}));
    CHECK(x == pound_force/inch/inch);
    CHECK(parse_quantity(x, {1.0, "pounds_per_inch2"}));
    CHECK(x == pound_force/inch/inch);
    CHECK(parse_quantity(x, {1.0, "pounds_per_square_inch"}));
    CHECK(x == pound_force/inch/inch);
    CHECK(parse_quantity(x, {1.0, "torr"}));
    CHECK(x == atmosphere / 760.0);
    CHECK(parse_quantity(x, {1.0, "atm"}));
    CHECK(x == atmosphere);
    CHECK(parse_quantity(x, {1.0, "atmosphere"}));
    CHECK(x == atmosphere);
}

TEST_CASE("si.format.energy")
{
    si::Energy x;
    CHECK(parse_quantity(x, {1.0, "kW_hr"}));
    CHECK(x == 1e3*watt*hour);
    CHECK(parse_quantity(x, {1.0, "kW_h"}));
    CHECK(x == 1e3*watt*hour);
    CHECK(parse_quantity(x, {1.0, "kWh"}));
    CHECK(x == 1e3*watt*hour);
    CHECK(parse_quantity(x, {1.0, "erg"}));
    CHECK(x == erg);
    CHECK(parse_quantity(x, {1.0, "foot_pound"}));
    CHECK(x == foot * pound_force);
    CHECK(parse_quantity(x, {1.0, "ft_lb"}));
    CHECK(x == foot * pound_force);
    CHECK(parse_quantity(x, {1.0, "ft_lbf"}));
    CHECK(x == foot * pound_force);
    CHECK(parse_quantity(x, {1.0, "BTU"}));
    CHECK(x == 1055.06 * joule);
}

TEST_CASE("si.format.power")
{
    si::Power x;
    CHECK(parse_quantity(x, {1.0, "hp"}));
    CHECK(x == 33000. * foot * pound_force / minute);
}

TEST_CASE("si.format.area")
{
    si::Area x;
    CHECK(parse_quantity(x, {1.0, "acre"}));
    CHECK(x == 43560. * foot * foot);
    CHECK(parse_quantity(x, {1.0, "ac"}));
    CHECK(x == 43560. * foot * foot);
    CHECK(parse_quantity(x, {1.0, "sq_mi"}));
    CHECK(x == mile * mile);
    CHECK(parse_quantity(x, {1.0, "mi^2"}));
    CHECK(x == mile * mile);
    CHECK(parse_quantity(x, {1.0, "mile2"}));
    CHECK(x == mile * mile);
    CHECK(parse_quantity(x, {1.0, "sq_ft"}));
    CHECK(x == foot * foot);
    CHECK(parse_quantity(x, {1.0, "ft^2"}));
    CHECK(x == foot * foot);
    CHECK(parse_quantity(x, {1.0, "foot2"}));
    CHECK(x == foot * foot);
    CHECK(parse_quantity(x, {1.0, "are"}));
    CHECK(x == 1e2*meter2);
    CHECK(parse_quantity(x, {1.0, "a"}));
    CHECK(x == 1e2*meter2);
    CHECK(parse_quantity(x, {1.0, "ha"}));
    CHECK(x == 1e4 * meter2);
    CHECK(parse_quantity(x, {1.0, "hectare"}));
    CHECK(x == 1e4 * meter2);
}

TEST_CASE("si.format.volume")
{
    si::Volume x;
    CHECK(parse_quantity(x, {1.0, "cc"}));
    CHECK(x == 1e-6 * meter3);
    CHECK(parse_quantity(x, {1.0, "L"}));
    CHECK(x == liter);
    CHECK(parse_quantity(x, {1.0, "liter"}));
    CHECK(x == liter);
    CHECK(parse_quantity(x, {1.0, "litre"}));
    CHECK(x == liter);
    CHECK(parse_quantity(x, {1.0, "gal"}));
    CHECK(x == gallon);
    CHECK(parse_quantity(x, {1.0, "gallon"}));
    CHECK(x == gallon);
    CHECK(parse_quantity(x, {1.0, "acre_ft"}));
    CHECK(x == 43560. * dim::pow<3>(foot) );
    CHECK(parse_quantity(x, {1.0, "acre_foot"}));
    CHECK(x == 43560. * dim::pow<3>(foot));
    CHECK(parse_quantity(x, {1.0, "cu_ft"}));
    CHECK(x == dim::pow<3>(foot));
    CHECK(parse_quantity(x, {1.0, "in^3"}));
    CHECK(x == dim::pow<3>(inch));
    CHECK(parse_quantity(x, {1.0, "cubic_inch"}));
    CHECK(x == dim::pow<3>(inch));
    CHECK(parse_quantity(x, {1.0, "cu_yd"}));
    CHECK(x == dim::pow<3>(yard));
    CHECK(parse_quantity(x, {1.0, "yd^3"}));
    CHECK(x == dim::pow<3>(yard));
    CHECK(parse_quantity(x, {1.0, "cubic_yard"}));
    CHECK(x == dim::pow<3>(yard));   
}

TEST_CASE("si.format.flow_rate")
{
    si::FlowRate x;
    CHECK(parse_quantity(x, {1.0, "gal/s"}));
    CHECK(x == gallon/second);
    CHECK(parse_quantity(x, {1.0, "gal/min"}));
    CHECK(x == gallon/minute);
    CHECK(parse_quantity(x, {1.0, "meter3/second"}));
    CHECK(x == meter3/second);
    CHECK(parse_quantity(x, {1.0, "liter/second"}));
    CHECK(x == liter/second);
    CHECK(parse_quantity(x, {1.0, "L/s"}));
    CHECK(x == liter/second);
    CHECK(parse_quantity(x, {1.0, "gallon/second"}));
    CHECK(x == gallon/second);
    CHECK(parse_quantity(x, {1.0, "gallon/minute"}));
    CHECK(x == gallon/minute);
}

TEST_CASE("si.format.speed")
{
    si::Speed x;
    CHECK(parse_quantity(x, {1.0, "mps"}));
    CHECK(x == meter/second);
    CHECK(parse_quantity(x, {1.0, "kph"}));
    CHECK(x == 1e3 * meter/hour);
    CHECK(parse_quantity(x, {1.0, "mph"}));
    CHECK(x == mile/hour);
    CHECK(parse_quantity(x, {1.0, "miles_per_hour"}));
    CHECK(x == mile/hour);
    CHECK(parse_quantity(x, {1.0, "knot"}));
    CHECK(x == knot);
    CHECK(parse_quantity(x, {1.0, "kn"}));
    CHECK(x == knot);
    CHECK(parse_quantity(x, {1.0, "kt"}));
    CHECK(x == knot);
    CHECK(parse_quantity(x, {1.0, "ft/s"}));
    CHECK(x == foot/second);
    CHECK(parse_quantity(x, {1.0, "feet_per_second"}));
    CHECK(x == foot/second);
}

TEST_CASE("si.format.acceleration")
{
    si::Acceleration x;
    CHECK(parse_quantity(x, {1.0, "ft/s^2"}));
    CHECK(x == foot/second2);
    CHECK(parse_quantity(x, {1.0, "feet_per_second2"}));
    CHECK(x == foot/second2);
}

TEST_CASE("si.format.angular_rate")
{
    si::AngularRate x;
    CHECK(parse_quantity(x, {1.0, "deg/s"}));
    CHECK(x == degree/second);
    CHECK(parse_quantity(x, {1.0, "°/s"}));
    CHECK(x == degree/second);
    CHECK(parse_quantity(x, {1.0, "degrees_per_second"}));
    CHECK(x == degree/second);
    CHECK(parse_quantity(x, {1.0, "radians_per_second"}));
    CHECK(x == radian/second);
}

TEST_CASE("si.format.angular_acceleration")
{
    si::AngularAcceleration x;
    CHECK(parse_quantity(x, {1.0, "deg/s^2"}));
    CHECK(x == degree / second2);
    CHECK(parse_quantity(x, {1.0, "°/s^2"}));
    CHECK(x == degree / second2);
    CHECK(parse_quantity(x, {1.0, "degrees_per_second2"}));
    CHECK(x == degree / second2);
    CHECK(parse_quantity(x, {1.0, "radians_per_second2"}));
    CHECK(x == radian / second2);
}

TEST_CASE("si.format.torque")
{
    si::Torque x;
    CHECK(parse_quantity(x, {1.0, "ft_lb"}));
    CHECK(x == foot * pound_force / radian);
    CHECK(parse_quantity(x, {1.0, "ft_lbf"}));
    CHECK(x == foot * pound_force / radian);
    CHECK(parse_quantity(x, {1.0, "foot_pound"}));
    CHECK(x == foot * pound_force / radian);
}