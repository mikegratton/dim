#include <cstring>

#include "dim/si.hpp"
#include "doctest.h"

TEST_CASE("ParserErrors")
{
    char buffer[64];
    buffer[0] = 0;
    using namespace dim::detail;
    auto result = parse_standard_rep<si::system, double>(buffer, -1);
    CHECK(result.is_bad() == true);
    buffer[0] = '(';
    buffer[1] = 0;
    result = parse_standard_rep<si::system, double>(buffer, -1);
    CHECK(result.is_bad() == true);
    buffer[1] = 'x';
    result = parse_standard_rep<si::system, double>(buffer, 63);
    CHECK(result.is_bad() == true);

    // Trash
    for (int i = 0; i < 64; i++) { buffer[i] = 127 + i; }
    result = parse_standard_rep<si::system, double>(buffer, sizeof(buffer));
    CHECK(result.is_bad() == true);
    result = parse_standard_rep<si::system, double>(buffer, -1);
    CHECK(result.is_bad() == true);

    // Bad parens
    for (int i = 0; i < 8; i++) { buffer[i] = '('; }
    buffer[8] = 0;
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);

    // Bad exponent
    strcpy(buffer, "m^(s)");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);

    // Bad symbol
    strcpy(buffer, "ft");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);

    // Bad math
    strcpy(buffer, "m**s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);
    strcpy(buffer, "m*/s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);
    strcpy(buffer, "m*2");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);

    // Operators
    strcpy(buffer, "m s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);
    strcpy(buffer, "m*s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);
    strcpy(buffer, "m_s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);
    strcpy(buffer, "m/s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);

    // Exponents
    strcpy(buffer, "m^+121");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);
    CHECK(result.unit(0) == 121);
    strcpy(buffer, "m^(-121)");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);
    CHECK(result.unit(0) == -121);
    strcpy(buffer, "m^3");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == false);
    CHECK(result.unit(0) == 3);
    strcpy(buffer, "m^s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);
    strcpy(buffer, "m^1.2");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);

    strcpy(buffer, "m,s");
    result = parse_standard_rep<si::system, double>(buffer);
    CHECK(result.is_bad() == true);
}
