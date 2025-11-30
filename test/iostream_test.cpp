#include "dim/si/definition.hpp"
#include "doctest.h"
#include "dim/si.hpp"
#include <sstream>

TEST_CASE("io_stream")
{
    using namespace dim;
    using namespace dim::si::literal;
    si::system::install_facet();

    std::stringstream ss;
    std::locale loc = std::locale{};
    ss.imbue(loc);
    
    ss << 5.0_m;
    CHECK(ss.str() == "5_m");
    ss.str("");
    si::Time time = 5.0_s;
    CHECK(index<si::Time>() == index(time));
    CHECK(index<si::Time>() != index<si::Temperature>());
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
