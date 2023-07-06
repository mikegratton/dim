#include <cstdlib>
#include <iostream>
#include <sstream>

#include "dim/si.hpp"
#include "dim/si/definition.hpp"
#include "doctest.h"

using namespace dim::si;
using namespace dim;

void doParseCheck(char const* text, double value, dim::dynamic_unit const& unit)
{
    char* endptr;
    double v = strtod(text, &endptr);
    endptr++;
    using namespace dim::detail;
    auto q = v * dim::detail::parse_standard_rep<dim::si::system, double>(endptr, strlen(endptr));
    CHECK(q.is_bad() == false);
    if (q.is_bad()) {
        std::cout << "Failed to parse " << text << ", got v = " << v << ", unit = " << endptr << std::endl;
    }
    CHECK(q.value() == doctest::Approx(value));
    CHECK(dim::unitsMatch(q.unit(), unit));
}

void doParseFailCheck(char const* text)
{
    using namespace dim::detail;
    auto q = dim::detail::parse_standard_rep<dim::si::system, double>(text, strlen(text));
    CHECK(q.is_bad() == true);
}

TEST_CASE("SiParse")
{
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
    doParseCheck("1_m^2*s^-1", 1.0, {2, -1, 0, 0, 0, 0, 0});

    doParseCheck("1_m^2*s^(-1)", 1.0, {2, -1, 0, 0, 0, 0, 0});

    doParseCheck("1_(m^2*s^-1)", 1.0, {2, -1, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2 s^-1", 1.0, {2, -1, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2/s", 1.0, {2, -1, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2/s^2", 1.0, {2, -2, 0, 0, 0, 0, 0});
    doParseCheck("1 m^2/s^-2", 1.0, {2, 2, 0, 0, 0, 0, 0});
    doParseCheck("1_m^((2))*s^-1", 1.0, {2, -1, 0, 0, 0, 0, 0});
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
    doParseCheck("1_a", 1e2, {2, 0, 0, 0, 0, 0, 0, 0});
    doParseCheck("1_bar", 1e5, {-1, -2, 1, 0, 0, 0, 0, 0});
}

TEST_CASE("PrintUnit")
{
    using namespace dim;
    dim::dynamic_unit u;
    char buf[256];

    u = {1, 0, 0, 0, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("m") == buf);

    u = {0, 1, 0, 0, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("s") == buf);

    u = {0, 0, 1, 0, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("kg") == buf);

    u = {0, 0, 0, 1, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("rad") == buf);

    u = {0, 0, 0, 0, 1, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("K") == buf);

    u = {0, 0, 0, 0, 0, 1, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("mol") == buf);

    u = {0, 0, 0, 0, 0, 0, 1, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("A") == buf);

    u = {0, 0, 0, 0, 0, 0, 0, 1};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("cd") == buf);

    // Compound
    u = {1, -1, 0, 0, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("m_s^-1") == buf);
    u = {1, -2, 0, -3, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("m_s^-2_rad^-3") == buf);

    u = {5, 0, 0, 0, 0, 0, 0, 0};
    print_unit<si::system>(buf, u, false);
    CHECK(std::string("m^5") == buf);
}

TEST_CASE("Formatter")
{
    auto f = dim::formatter<si::Length>("in", dim::si::inch);
    auto l = 2.0 * dim::si::meter;
    double in_inch = f.non_dim(l);
    CHECK(l / dim::si::inch == doctest::Approx(in_inch));
    auto ff = f.output(l);
    CHECK(ff.value() == doctest::Approx(in_inch));
    CHECK(strcmp(ff.symbol(), "in") == 0);
    sprintf(ff.set_symbol(), "%s", "moose");
    CHECK(strcmp(ff.symbol(), "moose") == 0);
    dim::si::Length l2 = f.input(in_inch);
    CHECK(dimensionless_cast(l2) == doctest::Approx(dimensionless_cast(l)));
}

TEST_CASE("OStream")
{
    using namespace dim::si::literal;
    std::stringstream ss;
    std::locale loc = std::locale(std::locale::classic(), dim::si::system::make_default_facet());
    ss.imbue(loc);

    ss << 5.0_m;
    CHECK(ss.str() == "5_m");
    ss.str("");
    si::Time time = 5.0_s;
    CHECK(si::Time::index() == time.index());
    CHECK(si::Time::index() != si::Temperature::index());
    ss << time;
    CHECK(ss.str() == "5_s");

    ss.str("");
    ss << 5.0_rad;
    CHECK(ss.str() == "5_rad");

    ss.str("");
    ss << 5.0_K;
    CHECK(ss.str() == "5_K");

    ss.str("");
    ss << 5.0_kg;
    CHECK(ss.str() == "5_kg");

    ss.str("");
    ss << 5.0_mol;
    CHECK(ss.str() == "5_mol");

    ss.str("");
    ss << 5.0_A;
    CHECK(ss.str() == "5_A");

    ss.str("");
    ss << 5.0_cd;
    CHECK(ss.str() == "5_cd");

    using namespace dim::si;
    ss.str("");
    ss << 5.0 * meter * kilogram / second2;
    CHECK(ss.str() == "5_N");

    ss.str("");
    ss << 5.0 / second;
    dim::si::Length L = 5.0_m;
    CHECK(ss.str() == "5_Hz");

    ss.str("");
    ss << 5.0 * radian * radian;
    CHECK(ss.str() == "5_sr");

    ss.str("");
    ss << 5.0 * pascal;
    CHECK(ss.str() == "5_Pa");

    ss.str("");
    ss << 5.0 * joule;
    CHECK(ss.str() == "5_J");

    ss.str("");
    ss << 5.0 * watt;
    CHECK(ss.str() == "5_W");

    ss.str("");
    ss << 5.0 * volt;
    CHECK(ss.str() == "5_V");

    ss.str("");
    ss << 5.0 * farad;
    CHECK(ss.str() == "5_F");

    ss.str("");
    ss << 5.0 * ohm;
    CHECK(ss.str() == "5_R");

    ss.str("");
    ss << 5.0 * siemens;
    CHECK(ss.str() == "5_S");

    ss.str("");
    ss << 5.0 * weber;
    CHECK(ss.str() == "5_Wb");

    ss.str("");
    ss << 5.0 * tesla;
    CHECK(ss.str() == "5_T");

    ss.str("");
    ss << 5.0 * henry;
    CHECK(ss.str() == "5_H");

    ss.str("");
    ss << 5.0 * lumen;
    CHECK(ss.str() == "5_Im");

    ss.str("");
    ss << 5.0 * lux;
    CHECK(ss.str() == "5_Ix");

    ss.str("");
    ss << 5.0 * katal;
    CHECK(ss.str() == "5_kat");

    ss.str("");
    ss << 5.0 * poiseuille;
    CHECK(ss.str() == "5_Pl");
}

TEST_CASE("StringFormat")
{
    std::string text = "5.2_deg";
    dim::si::Angle angle;
    CHECK(from_string(angle, text));
    CHECK(angle / dim::si::degree == doctest::Approx(5.2));
    auto text2 = to_string(angle);
    CHECK(text2 == "0.090757_rad");
}

TEST_CASE("LengthFormat")
{
}
// ...
