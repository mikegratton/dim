#include "doctest.h"
#include "dim/si/definition.hpp"
#include "dim/si/literal.hpp"

using namespace dim::si::literal;

TEST_CASE("EquivalentLiteralTest") {
    auto x = 1_m;
    auto y = 1.0_m;
    auto z = 1.0 * dim::si::meter;
    auto w = dim::si::Length(1.0);
    CHECK(x == y);
    CHECK(x == z);
    CHECK(x == w);
}

TEST_CASE("SiLiterals") {
    using namespace dim::si;
    CHECK(1_m == 1.0*meter);
    CHECK(1_s == 1.0*second);
    CHECK(1_kg == 1.0*kilogram);
    CHECK(1_rad == 1.0*radian);
    CHECK(1_K == 1.0*kelvin);
    CHECK(1_mol == 1.0*mole);
    CHECK(1_A == 1.0*ampere);
    CHECK(1_cd == 1.0*candela);

    CHECK(1_sr == 1.0*dim::si::steradian);
    CHECK(1_Hz == 1.0*dim::si::hertz);
    CHECK(1_N == 1.0*dim::si::newton);
    CHECK(1_Pa == 1.0*dim::si::pascal);
    CHECK(1_J == 1.0*dim::si::joule);
    CHECK(1_W == 1.0*dim::si::watt);
    CHECK(1_C == 1.0*dim::si::coulomb);
    CHECK(1_V == 1.0*dim::si::volt);
    CHECK(1_F == 1.0*dim::si::farad);
    CHECK(1_R == 1.0*dim::si::ohm);
    CHECK(1_S == 1.0*dim::si::siemens);
    CHECK(1_Wb == 1.0*dim::si::weber);
    CHECK(1_T == 1.0*dim::si::tesla);
    CHECK(1_H == 1.0*dim::si::henry);
    CHECK(1_Im == 1.0*dim::si::lumen);
    CHECK(1_Ix == 1.0*dim::si::lux);
    CHECK(1_kat == 1.0*dim::si::katal);
}
