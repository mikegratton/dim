#include "doctest.h"
#include "dim/si.hpp"

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

    text = "not right";
    CHECK_FALSE(from_string(angle, text));
    CHECK(angle.is_bad());

    text = "1deg";
    CHECK(from_string(angle, text));
    CHECK(angle / dim::si::degree == doctest::Approx(1.0));

    text = "1degdegdeg";
    CHECK_FALSE(from_string(angle, text));
    CHECK(angle.is_bad());

    text = "1deg*";
    CHECK_FALSE(from_string(angle, text));
    CHECK(angle.is_bad());
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
    // Static quantity, float formats
    auto message = std::format("{:.3f}", M_SQRT2 * si::meter);
    CHECK(message == "1.414_m");

    // dynamic_quantity
    message = std::format("{}", si::dynamic_quantity(2.0 * si::weber));
    CHECK(message == "2_Wb");

    // Check we're not crowding out other types
    message = std::format("Lots of {} go in {} pen for {}", "sheep", 1.0, si::second);
    CHECK(message == "Lots of sheep go in 1 pen for 1_s");

    // Check formatted quantity
    si::formatter f("in", si::inch);
    auto fq = f.output(si::yard);
    message = std::format("One yard is {:.0f}", fq);
    CHECK(message == "One yard is 36_in");
}
#endif