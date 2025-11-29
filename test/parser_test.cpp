#include <cstring>

#include "dim/si.hpp"
#include "dim/si/definition.hpp"
#include "doctest.h"

using namespace dim::detail;

TEST_CASE("quantity_parser.empty")
{
    char buffer[64];
    buffer[0] = 0;    
    auto result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK_FALSE(result.is_bad());
    CHECK(result.dimensionless());
    CHECK(result.value() == 1.0);
}

TEST_CASE("quantity_parser.utf8")
{
    // Greek omega
    char buffer[64];
    sprintf(buffer, "kΩ");
    auto result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK(result.value() == 1000.0);
    CHECK(result.unit() == dim::index<si::Resistance>());

    // Greek mu
    sprintf(buffer, "μm/s");
    result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK(result.value() == 1e-6);
    CHECK(result.unit() == dim::index<si::Speed>());

    // Kelvin sign (not actually 'K')
    sprintf(buffer, "mK");
    result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK(result.value() == 1e-3);
    CHECK(result.unit() == dim::index<si::Temperature>());
    
    // Ohm sign (not actually capital omega)
    sprintf(buffer, "μΩ");
    result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK(result.value() == 1e-6);
    CHECK(result.unit() == dim::index<si::Resistance>());
}

TEST_CASE("quantity_parser.errors")
{
    char buffer[64];
    buffer[0] = '(';
    buffer[1] = 0;
    auto result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK(result.is_bad() == true);
    buffer[1] = 'x';
    result = parse_standard_rep<double, si::system>(buffer, buffer + 63);
    CHECK(result.is_bad() == true);

    // Trash
    for (int i = 0; i < 64; i++) { buffer[i] = 127 + i; }
    result = parse_standard_rep<double, si::system>(buffer, buffer + sizeof(buffer));
    CHECK(result.is_bad() == true);
    result = parse_standard_rep<double, si::system>(buffer, buffer + 64);
    CHECK(result.is_bad() == true);

    // Bad parens
    for (int i = 0; i < 8; i++) { buffer[i] = '('; }
    buffer[8] = 0;
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);

    // Bad exponent
    strcpy(buffer, "m^(s)");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);

    // Bad symbol
    strcpy(buffer, "ft");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);

    // Bad math
    strcpy(buffer, "m**s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);
    strcpy(buffer, "m*/s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);
    strcpy(buffer, "m*2");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);

    // Operators
    strcpy(buffer, "m s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);
    strcpy(buffer, "m*s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);
    strcpy(buffer, "m_s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);
    strcpy(buffer, "m/s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);

    // Exponents
    strcpy(buffer, "m^+121");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);
    CHECK(result.unit().length() == 121);
    strcpy(buffer, "m^(-121)");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);
    CHECK(result.unit().length() == -121);
    strcpy(buffer, "m^3");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == false);
    CHECK(result.unit().length() == 3);
    strcpy(buffer, "m^s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);
    strcpy(buffer, "m^1.2");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);

    strcpy(buffer, "m,s");
    result = parse_standard_rep<double, si::system>(buffer,  buffer + 64);
    CHECK(result.is_bad() == true);
}
