#include <cstdlib>
#include <iostream>
#include <locale>
#include <sstream>

#include "dim/dynamic_quantity.hpp"
#include "dim/io_format.hpp"
#include "dim/si/literal.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/si_quantity_facet.hpp"
#include "doctest.h"
#include "dim/io_stream.hpp"

using namespace dim::si;
using namespace dim;

void doParseCheck(char const* text, double value, si::dynamic_unit const& unit)
{
    char* endptr;
    double v = strtod(text, &endptr);
    endptr++;
    using namespace dim::detail;
    auto q = v * dim::detail::parse_standard_rep<double, dim::si::system>(endptr, strlen(endptr));
    CHECK(q.is_bad() == false);
    if (q.is_bad()) {
        std::cout << "Failed to parse " << text << ", got v = " << v << ", unit = " << endptr << std::endl;
    }
    CHECK(q.value() == doctest::Approx(value));
    CHECK(q.unit() == unit);
}

void doParseFailCheck(char const* text)
{
    using namespace dim::detail;
    auto q = dim::detail::parse_standard_rep<double, dim::si::system>(text, strlen(text));
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
}

TEST_CASE("print_unit")
{
    char buf[256];

    detail::print_unit(buf, index(si::meter), false);
    CHECK(std::string("m") == buf);

    detail::print_unit(buf, index(si::second), false);
    CHECK(std::string("s") == buf);


    detail::print_unit(buf, index(si::kilogram), false);
    CHECK(std::string("kg") == buf);


    detail::print_unit(buf, index(si::radian), false);
    CHECK(std::string("rad") == buf);


    detail::print_unit(buf, index(si::kelvin), false);
    CHECK(std::string("K") == buf);


    detail::print_unit(buf, index(si::mole), false);
    CHECK(std::string("mol") == buf);


    detail::print_unit(buf, index(si::ampere), false);
    CHECK(std::string("A") == buf);


    detail::print_unit(buf, index(si::candela), false);
    CHECK(std::string("cd") == buf);

    // Compound

    detail::print_unit(buf, index<si::Speed>(), false);
    CHECK(std::string("m_s^-1") == buf);

    auto q = si::meter / pow<2>(si::second) / pow<3>(si::radian);
    detail::print_unit(buf, index(q), false);
    CHECK(std::string("m_s^-2_rad^-3") == buf);

    detail::print_unit(buf, pow(index(si::meter), 5), false);
    CHECK(std::string("m^5") == buf);
}

// TODO
TEST_CASE("LengthFormat")
{
}
// ...
