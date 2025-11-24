#include "dim/dynamic_quantity.hpp"
#include "dim/io.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/si_quantity_facet.hpp"
#include "dim/tag.hpp"
#include "doctest.h"
#include "dim/si.hpp"
#include "dim/io_format.hpp"
#include <concepts>
#include <format>
#include <locale>

using namespace dim::si::literal;

TEST_CASE("from_string")
{
    si::system::install_facet();
    std::string text = "5.2_deg";
    dim::si::Angle angle;
    CHECK(from_string(angle, text));
    CHECK(angle / dim::si::degree == doctest::Approx(5.2));
    auto text2 = to_string(angle);
    CHECK(text2 == "0.090757_rad");

    std::string text3 = "12*kN/m";
    decltype(si::newton / si::meter) result;
    CHECK(from_string(result, text3));
}

TEST_CASE("to_string")
{
    si::system::install_facet();
    si::Angle angle = 5.2 * si::degree;
    std::string text = to_string(angle);
    CHECK(text == "0.090757_rad");    
}

#if __cplusplus >= 202002L
TEST_CASE("std_format")
{
    //si::system::install_facet();
    std::cout << std::format("This is a dynamic quantity: {}\n", si::dynamic_quantity(5.0_m));
    std::cout << std::format("This is a length: {}\n", 5.0_m);
}
#endif