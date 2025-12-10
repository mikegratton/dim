#include "doctest.h"
#include "dim/si.hpp"

TEST_CASE("quantity.add")
{
    si::Length x(1.0);
    si::Length y(2.0);

    // Add/subtract
    CHECK((x+y)/si::meter_ == doctest::Approx(3.0));
    CHECK((x-y)/si::meter_ == doctest::Approx(-1.0));
    x += y;
    CHECK(x/si::meter_ == doctest::Approx(3.0));
    x -= y;
    CHECK(x/si::meter_ == doctest::Approx(1.0));
    x += si::meter_;
    CHECK(x/si::meter_ == doctest::Approx(2.0));
    x -= si::meter_;
    CHECK(x/si::meter_ == doctest::Approx(1.0));
    y = x + si::meter_;
    CHECK(y/si::meter_ == doctest::Approx(2.0));
    y = x - si::meter_;
    CHECK(y/si::meter_ == doctest::Approx(0.0));

}

TEST_CASE("quantity.multiply")
{
    si::Length x(1.0);
    si::Length y(2.0);
    // Multiply/divide
    x = 3.0 * si::meter;
    si::Area a = x*y;
    CHECK(a/si::meter_/si::meter_ == doctest::Approx(6.0));
    si::Length z = a / x;
    CHECK(z/si::meter_ == doctest::Approx(y/si::meter_));
    x *= 2.0;
    CHECK(x/si::meter == doctest::Approx(6.0));
    x /= 2.0;
    CHECK(x/si::meter_ == doctest::Approx(3.0));
    y = x * 2.0;
    CHECK(y/si::meter_ == doctest::Approx(6.0));
    y = 2.0 * x;
    CHECK(y/si::meter_ == doctest::Approx(6.0));
    y = x/2.0;
    CHECK(y/si::meter_ == doctest::Approx(1.5));
    auto w = 2.0/x;
    CHECK(w*si::meter_ == doctest::Approx(2./3.));

    w = 2.0/si::meter_;
    CHECK(w*si::meter == 2.0);
    y = si::meter_ / 2.0;
    CHECK(y/si::meter_ == 0.5);
    y = si::meter_ * 2.0;
    CHECK(y/si::meter_ == 2.0);
}

TEST_CASE("quantity.compare")
{
    si::Length x = 1.0 * si::meter;
    si::Length y = 2.0 * si::meter;
    CHECK(y > x);
    CHECK(x < y);
    CHECK(y >= x);
    CHECK(x <= y);
    CHECK(x == x);
    CHECK(x != y);
    CHECK(x <= x);
    CHECK(x >= x);
}

TEST_CASE("quantity.misc")
{
    si::Length y = 2.0 * si::meter;
    si::Area a = si::pow<2>(y);
    CHECK(dimensionless_cast(a) == 4.0);
    si::Length x = si::ratpow<1,2>(a);
    CHECK(x / si::meter_ == y / si::meter_);
    x = si::sqrt(a);
    CHECK(x / si::meter_ == y / si::meter_);
    x = si::root<2>(a);
    CHECK(x / si::meter_ == y / si::meter_);
    x *= -1.0;
    CHECK(si::abs(x) == y);
}

