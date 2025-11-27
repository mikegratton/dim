#include <cstring>
#include <sstream>
#include "dim/io_detail.hpp"
#include "dim/si.hpp"
#include "doctest.h"

TEST_CASE("extract_quantity")
{
    si::formatted_quantity formatted;
    {
        std::stringstream ss;
        ss.str("+123.456E-123_m^2");        
        bool status = dim::detail::extract_formatted_quantity(formatted, ss);
        CHECK(status == true);
        CHECK(std::string(formatted.symbol()) == "m^2");
        CHECK(formatted.value() == +123.456E-123);        
    }
    {
        std::stringstream ss;
        ss.str("12a3 km");        
        bool status = dim::detail::extract_formatted_quantity(formatted, ss);
        CHECK(status == true);
        CHECK(formatted.value() == 12);
        CHECK(std::string(formatted.symbol()) == "a");
    }
    {
        std::stringstream ss;
        ss.str("12e+-1 km");        
        bool status = dim::detail::extract_formatted_quantity(formatted, ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("1+2e km");        
        bool status = dim::detail::extract_formatted_quantity(formatted, ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("1e km");        
        bool status = dim::detail::extract_formatted_quantity(formatted, ss);
        CHECK(status == false);
    }
    {
        std::stringstream ss;
        ss.str("1_km");        
        bool status = dim::detail::extract_formatted_quantity(formatted, ss);
        CHECK(status == true);        
        CHECK(std::string(formatted.symbol()) == "km");
        CHECK(formatted.value() == 1);  
    }
}

TEST_CASE("print_unit")
{
    char buf[1024];
    char* cursor = print_unit(buf, buf + 1024, si::dynamic_unit(si::meter_));
    CHECK(std::string(buf) == "m");
    CHECK(cursor == buf+1);
}

TEST_CASE("print_int8")
{
    char buf[8];
    memset(buf, 0, 8);
    int8_t d = -128;
    char* cursor = dim::detail::print_int8(buf, d);
    CHECK(cursor - buf == 4);
    CHECK(std::string(buf) == "-128");
    
    d = 0;
    memset(buf, 0, 8);
    cursor = dim::detail::print_int8(buf, d);
    CHECK(std::string(buf) == "0");
    CHECK(cursor - buf == 1);

    d = 127;
    memset(buf, 0, 8);
    cursor = dim::detail::print_int8(buf, d);
    CHECK(std::string(buf) == "127");
    CHECK(cursor - buf == 3);

    d = 12;
    memset(buf, 0, 8);
    cursor = dim::detail::print_int8(buf, d);
    CHECK(std::string(buf) == "12");
    CHECK(cursor - buf == 2);
}

// todo print_unit
// todo io_detail