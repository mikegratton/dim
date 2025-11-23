#include <cstring>
#include <sstream>

#include "dim/io_stream.hpp"
#include "dim/si.hpp"
#include "doctest.h"

TEST_CASE("extract_quantity")
{
    {
        std::stringstream ss;
        ss.str("+123.456E-123_m^2");
        char buf[64];
        char* unit;
        bool status = dim::detail::extract_quantity(buf, unit, sizeof(buf), ss);
        CHECK(status == true);
        CHECK(strcmp(unit, "m^2") == 0);
        CHECK(strcmp(buf, "+123.456E-123") == 0);
    }
    {
        std::stringstream ss;
        ss.str("12a3 km");
        char buf[64];
        char* unit;
        bool status = dim::detail::extract_quantity(buf, unit, sizeof(buf), ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("12e+-1 km");
        char buf[64];
        char* unit;
        bool status = dim::detail::extract_quantity(buf, unit, sizeof(buf), ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("1+2e km");
        char buf[64];
        char* unit;
        bool status = dim::detail::extract_quantity(buf, unit, sizeof(buf), ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("1e km");
        char buf[64];
        char* unit;
        bool status = dim::detail::extract_quantity(buf, unit, sizeof(buf), ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("1_km");
        char buf[64];
        char* unit;
        bool status = dim::detail::extract_quantity(buf, unit, sizeof(buf), ss);
        CHECK(status == true);
        CHECK(strcmp(buf, "1") == 0);
        CHECK(strcmp(unit, "km") == 0);
    }
}