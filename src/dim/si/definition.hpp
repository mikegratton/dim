#pragma once
#include "dim/base.hpp"
#include "dim/system_creation_helper.hpp"

namespace dim
{

class quantity_facet;

namespace si
{

namespace symbol
{
template<class U>
constexpr const char* specialized_symbol() { return "\0"; }
}

struct system : system_tag {
    static const long id = 26980L;
    static const char* SYMBOL[];
    constexpr static const char* symbol_for(int dimension)
    {
        return SYMBOL[dimension];
    }

    template<class U>
    static constexpr const char* specialized_symbol()
    {
        return symbol::specialized_symbol<U>();
    }
    static quantity_facet* make_default_facet();
};

/*
 * Install the facet into the global locale. If specialized is nullptr,
 * then install the default facet for si.
 */
void add_to_global_locale(quantity_facet* specialized = nullptr);

// Fundamental dimensions to types and base unit definitions
DEFINE_QUANTITY_S(Length,      meter,     system, double, 1, 0, 0, 0, 0, 0, 0, 0, "m")
DEFINE_QUANTITY_S(Time,        second,    system, double, 0, 1, 0, 0, 0, 0, 0, 0, "s")
DEFINE_QUANTITY_S(Mass,        kilogram,  system, double, 0, 0, 1, 0, 0, 0, 0, 0, "kg")
DEFINE_QUANTITY_S(Angle,       radian,    system, double, 0, 0, 0, 1, 0, 0, 0, 0, "rad")
DEFINE_QUANTITY_S(Temperature, kelvin,    system, double, 0, 0, 0, 0, 1, 0, 0, 0, "K")
DEFINE_QUANTITY_S(Amount,      mole,      system, double, 0, 0, 0, 0, 0, 1, 0, 0, "mol")
DEFINE_QUANTITY_S(Current,     ampere,    system, double, 0, 0, 0, 0, 0, 0, 1, 0, "A")
DEFINE_QUANTITY_S(Luminosity,  candela,   system, double, 0, 0, 0, 0, 0, 0, 0, 1, "cd")

// Derived units
DEFINE_QUANTITY_S(Frequency,           hertz,   system, double, 0, -1, 0, 0, 0, 0, 0, 0, "Hz")
DEFINE_QUANTITY_S(SolidAngle,       steradian,  system, double, 0, 0, 0, 2, 0, 0, 0, 0, "sr")
DEFINE_QUANTITY_S(Force,               newton,  system, double, 1, -2, 1, 0, 0, 0, 0, 0, "N")
DEFINE_QUANTITY_S(Pressure,            pascal,  system, double, -1, -2, 1, 0, 0, 0, 0, 0, "Pa")
DEFINE_QUANTITY_S(Energy,              joule,   system, double, 2, -2, 1, 0, 0, 0, 0, 0, "J")
DEFINE_QUANTITY_S(Power,               watt,    system, double, 2, -3, 1, 0, 0, 0, 0, 0, "W")
DEFINE_QUANTITY_S(Charge,              coulomb, system, double, 0, 1, 0, 0, 0, 0, 1, 0, "C")
DEFINE_QUANTITY_S(Voltage,             volt,    system, double, 2, -3, 1, 0, 0, 0, -1, 0, "V")
DEFINE_QUANTITY_S(Capacitance,         farad,   system, double, -2, 4, -1, 0, 0, 0, 1, 0, "F")
DEFINE_QUANTITY_S(Resistance,          ohm,     system, double, 2, -3, 1, 0, 0, 0, -2, 0, "R")
DEFINE_QUANTITY_S(Conductance,         siemens, system, double, -2, 3, -1, 0, 0, 0, 2, 0, "S")
DEFINE_QUANTITY_S(MagneticFlux,        weber,   system, double, 2, -2, 1, 0, 0, 0, -1, 0, "Wb")
DEFINE_QUANTITY_S(MagneticFluxDensity, tesla,   system, double, 0, -2, 1, 0, 0, 0, -1, 0, "T")
DEFINE_QUANTITY_S(Inductance,          henry,   system, double, 2, -2, 1, 0, 0, 0, -2, 0, "H")
DEFINE_QUANTITY_S(LuminousFlux,        lumen,   system, double, 0, 0, 0, 2, 0, 0, 0, 1, "Im")
DEFINE_QUANTITY_S(Luminance,           lux,     system, double, -2, 0, 0, 2, 0, 0, 0, 1, "Ix")
DEFINE_QUANTITY_S(CatalyticActivity,   katal,   system, double, 0, -1, 0, 0, 0, 1, 0, 0, "kat")
DEFINE_QUANTITY_S(Viscosity,           poiseuille, system, double, -1, -1, 1, 0, 0, 0, 0, 0, "Pl")

// Compound units without specialized unit names
DEFINE_QUANTITY(Area,                  meter2,  system, double, 2, 0, 0, 0, 0, 0, 0, 0)
DEFINE_QUANTITY(Volume,                meter3,  system, double, 3, 0, 0, 0, 0, 0, 0, 0)
constexpr unit_multiply_t<Time::unit, Time::unit> second2;

// Compound quantities with specialized names but not specialized units
using FlowRate            = quantity<unit_divide_t<Volume::unit, Time::unit>, double>;
using Speed               = quantity<unit_divide_t<Length::unit, Time::unit>, double>;
using Acceleration        = quantity<unit_divide_t<Speed::unit, Time::unit>, double>;
using AngularRate         = quantity<unit_divide_t<Angle::unit, Time::unit>, double>;
using AngularAcceleration = quantity<unit_divide_t<AngularRate::unit, Time::unit>, double>;
using Torque              = quantity<unit_divide_t<Energy::unit, Angle::unit>, double>;
using Density             = quantity<unit_divide_t<Mass::unit, Volume::unit>, double>;
using KinematicViscosity  = quantity<unit_divide_t<Area::unit, Time::unit>, double>;
/*****************************************************************************
 * CONVERSIONS
 *****************************************************************************
 */

// Common SI-allied units
constexpr Time minute = 60.0*second;
constexpr Time hour = 60.0*minute;
constexpr Area hectare = 1e4*meter2;
constexpr Volume liter = 1e-3*meter3;
constexpr Pressure bar = 1e5*pascal;
constexpr Pressure atmosphere = 1.01325e5*pascal;
constexpr Energy calorie = 4.184*joule;
constexpr Angle degree = 0.0174532925199433*radian;

// FPS
constexpr Length foot = 0.3048*meter;
constexpr Area feet2 = foot*foot;
constexpr Volume feet3 = foot*foot*foot;
constexpr Mass slug = 14.59390*kilogram;
constexpr Force pound_force = 4.4482216152605*newton;
//constexpr Pressure pounds_per_inch2 = pound_force/inch/inch;
constexpr Temperature rankine = 5./9.*kelvin;
// FPS allied units
constexpr Length inch = 0.0254*meter;
constexpr Length yard = 3.0*foot;
constexpr Length mile = 5280.0*foot;
constexpr Mass pound_mass = 0.453592*kilogram;
constexpr Force poundal = 0.138255*newton;
constexpr Area acre = 4046.86*meter2;
constexpr Volume gallon = 3.78541*liter;

// Nautical
constexpr Length nautical_mile = 1852.0*meter;
constexpr Speed knot = nautical_mile/hour;
// constexpr Angle arc_minute = degree/60.0;
// constexpr Angle arc_second = arc_minute/60.0;

// CGS
constexpr Length centimeter = 1e-2*meter;
constexpr Mass gram = 1e-3*kilogram;
constexpr Acceleration gal = centimeter/second/second;
constexpr Force dyne = 1e-5*newton;
constexpr Energy erg = 1e-7*joule;
constexpr Pressure barye = 1e-1*pascal;
constexpr Viscosity poise = 1e-1*pascal*second;
constexpr KinematicViscosity stokes = 1e-4*meter2/second;
constexpr MagneticFlux maxwell = 1e-8*weber;
constexpr MagneticFluxDensity gauss = 1e-4*tesla;
constexpr Luminance phot = 1e5*lux;

// Affine temperature conversions (Celsius/Fahrenheit)
constexpr inline Temperature celsius(double temp_c) { return (temp_c + 273.15)*kelvin; }
constexpr inline double toCelsiusValue(Temperature arg_t) { return (arg_t/kelvin - 273.15); }
constexpr inline Temperature fahrenheit(double temp_f) { return (temp_f + 459.67)*rankine; }
constexpr inline double toFahrenheitValue(Temperature arg_t) { return (arg_t/rankine - 459.67); }

/**********************************************************************
 * Trig function overloads inside dim::si
 *********************************************************************/

} // end of namespace si
} // end of namespace dim

#ifndef DIMENSION_NO_MATH
#include <cmath>
namespace dim
{
namespace si
{
inline double sin(Angle const& q) { return ::std::sin(dimensionless_cast(q)); }
inline double cos(Angle const& q) { return ::std::cos(dimensionless_cast(q)); }
inline double tan(Angle const& q) { return ::std::tan(dimensionless_cast(q)); }
inline Angle asin(double const& x) { return ::std::asin(x)*radian; }
inline Angle acos(double const& x) { return ::std::acos(x)*radian; }
inline Angle atan(double const& x) { return ::std::atan(x)*radian; }
template<class Q, DIM_IS_QUANTITY(Q)>
inline Angle atan2(Q const& x, Q const& y) { return ::std::atan2(dimensionless_cast(x), dimensionless_cast(y))*radian; }
inline Angle atan2(double const& x, double const& y) { return ::std::atan2(x, y)*radian; }
}
}
#endif

