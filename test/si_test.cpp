#include "dim/si/definition.hpp"
#undef DIM_NO_MATH

#include "doctest.h"
#include "dim/si.hpp"

using namespace dim::si;

namespace {
template<class Q, class Qp> void doCheck(Qp u, std::string symbol) {
    using U1 = typename Q::unit;
    using U2 = typename Qp::unit;
    DIM_CHECK_DIMENSIONS(U1, U2)
    CHECK(U2::system::id == dim::si::system::id);
    CHECK(symbol == system::specialized_symbol<U2>());
}
}

TEST_CASE("SiId")
{
    char const* id_string = "si";
    long value = (long) *reinterpret_cast<short const*>(id_string);
    CHECK(value == si::system::id);

}

TEST_CASE("SiFundamental") {
    doCheck<Length>(meter, "m");
    doCheck<Time>(second, "s");
    doCheck<Mass>(kilogram, "kg");
    doCheck<Angle>(radian, "rad");
    doCheck<Temperature>(kelvin, "K");
    doCheck<Amount>(mole, "mol");
    doCheck<Current>(ampere, "A");
    doCheck<Luminosity>(candela, "cd");
}

TEST_CASE("SiDerived") {
    doCheck<Frequency>(hertz, "Hz");
    doCheck<SolidAngle>(steradian, "sr");
    doCheck<Force>(newton, "N");
    doCheck<Pressure>(pascal, "Pa");
    doCheck<Energy>(joule, "J");
    doCheck<Power>(watt, "W");
    doCheck<Charge>(coulomb, "C");
    doCheck<Voltage>(volt, "V");
    doCheck<Capacitance>(farad, "F");
    doCheck<Resistance>(ohm, "Ω");
    doCheck<Conductance>(siemens, "S");
    doCheck<MagneticFlux>(weber, "Wb");
    doCheck<MagneticFluxDensity>(tesla, "T");
    doCheck<Inductance>(henry, "H");
    doCheck<LuminousFlux>(lumen, "Im");
    doCheck<Luminance>(lux, "Ix");
    doCheck<CatalyticActivity>(katal, "kat");
    doCheck<Viscosity>(poiseuille, "Pl");
}

TEST_CASE("CompoundUnits") {
    CHECK(1.0*meter2 == 1.0*meter*meter);
    CHECK(1.0*meter3 == 1.0*meter*meter*meter);
    CHECK(FlowRate(1) == 1.0*meter3/second);
    CHECK(Speed(1) == 1.0*meter/second);
    CHECK(Acceleration(1) == 1.0*meter/second/second);
    CHECK(AngularRate(1) == 1.0*radian/second);
    CHECK(AngularAcceleration(1) == 1.0*radian/second/second);
    CHECK(Torque(1) == 1.0*newton*meter/radian);
    CHECK(Density(1) == 1.0*kilogram/meter3);
    CHECK(KinematicViscosity(1) == 1.0*meter2/second);
}

TEST_CASE("AlliedUnits") {
    CHECK(minute == 60.0*second);
    CHECK(hour == 60.0*minute);
    CHECK(hectare == dim::pow<2>(100.*meter));
    CHECK(liter / dim::pow<3>(0.1*meter) == doctest::Approx(1.0));
    CHECK(bar == 1e5*pascal);
    CHECK(atmosphere/1.01325e5/pascal == doctest::Approx(1.0));
    CHECK(calorie/(4.184*joule) == doctest::Approx(1.0));
    CHECK(180.0*degree/(M_PI*radian) == doctest::Approx(1.0));
}

TEST_CASE("FpsUnits") {
    CHECK(foot == 0.3048*meter);
    CHECK(feet2/dim::pow<2>(foot) == doctest::Approx(1.0));
    CHECK(feet3/dim::pow<3>(foot) == doctest::Approx(1.0));
    CHECK(slug/(1.0*pound_force*second*second/foot) == doctest::Approx(1.0));
    CHECK(pound_force/(4.4482216152605*newton) == doctest::Approx(1.0));
    CHECK(rankine/(5./9. * kelvin) == doctest::Approx(1.0));
    CHECK(inch / (0.0254*meter) == doctest::Approx(1.0));
    CHECK(yard / (3.0*foot) == doctest::Approx(1.0));
    CHECK(mile / (5280.0*foot) == doctest::Approx(1.0));
    CHECK(pound_mass/(0.453592*kilogram) == doctest::Approx(1.0));
    CHECK(poundal/(pound_mass*foot/second/second) == doctest::Approx(1.0));
    CHECK(acre/(4046.86*meter2) == doctest::Approx(1.0));
    CHECK(gallon / (3.78541*liter) == doctest::Approx(1.0));
}

TEST_CASE("NauticalUnits") {
    CHECK(nautical_mile / (1852.*meter) == doctest::Approx(1.0));
    CHECK(knot/(nautical_mile/hour) == doctest::Approx(1.0));
}

TEST_CASE("CgsUnits") {
    CHECK(centimeter/(0.01*meter) == doctest::Approx(1.0));
    CHECK(gram/(0.001*kilogram) == doctest::Approx(1.0));
    CHECK(gal/(centimeter/second/second) == doctest::Approx(1.0));
    CHECK(dyne/(1e-5*newton) == doctest::Approx(1.0));
    CHECK(erg/(1e-7*joule) == doctest::Approx(1.0));
    CHECK(barye/(0.1*pascal) == doctest::Approx(1.0));
    CHECK(poise/(0.1*pascal*second) == doctest::Approx(1.0));
    CHECK(stokes/(1e-4*meter2/second) == doctest::Approx(1.0));
    CHECK(maxwell/(1e-8*weber) == doctest::Approx(1.0));
    CHECK(gauss/(1e-4*tesla) == doctest::Approx(1.0));
    CHECK(phot/(1e5*lux) == doctest::Approx(1.0));
}

TEST_CASE("Temperature") {
    CHECK(celsius(0.0)/(273.15*kelvin) == doctest::Approx(1.0));
    CHECK(celsius(1.0)/(274.15*kelvin) == doctest::Approx(1.0));
    CHECK(toCelsiusValue(celsius(0.0)) == doctest::Approx(0.0));
    CHECK(toCelsiusValue(celsius(1.0)) == doctest::Approx(1.0));
    CHECK(fahrenheit(32.0)/celsius(0.0) == doctest::Approx(1.0));
    CHECK(fahrenheit(33.0)/celsius(5./9.) == doctest::Approx(1.0));
    CHECK(toFahrenheitValue(fahrenheit(0.0)) == doctest::Approx(0.0));
    CHECK(toFahrenheitValue(fahrenheit(1.0)) == doctest::Approx(1.0));
}

TEST_CASE("Math") {
    CHECK(sin(0.0*radian) == doctest::Approx(0.0));
    CHECK(cos(0.0*radian) == doctest::Approx(1.0));
    CHECK(tan(0.0*radian) == doctest::Approx(0.0));
    CHECK(dim::si::asin(0.0)/radian == doctest::Approx(0.0));
    CHECK(dim::si::acos(1.0)/radian == doctest::Approx(0.0));
    CHECK(dim::si::atan(0.0)/radian == doctest::Approx(0.0));
    CHECK(dim::si::atan2(0.0, 1.0)/radian == doctest::Approx(0.0));
    CHECK(dim::si::atan2(0.0*meter, 1.0*meter)/radian == doctest::Approx(0.0));

}

TEST_CASE("Namespace")
{
    si::Length L = 5.0*si::meter;
    si::Area A = L*L;
    si::Length L2 = si::sqrt(A);
}
