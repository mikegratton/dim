#pragma once
#include "dim/quantity.hpp"
#include "dim/system_creation_helper.hpp"
#include "si_facet.hpp"

/*
 * This file defines the types making up the SI system. This includes
 * - The si::system type itself
 * - Types for each dimension (unit, quantity, and symbol)
 * - All of the derived units (unit, quantity, and symbol)
 * - typedefs for compound quantities such as si::Speed
 * - "Conversion" types giving the si measure of types from other system (such as si::foot)
 * - Trig functions for si::Angle types
 */

namespace dim
{

/// Namespace for the International System (SI) of units
namespace si
{

using dynamic_unit = ::dim::dynamic_unit<si::system>;

namespace symbol
{
/// If there's no specialized symbol for U, just return a null char
template<class U>
constexpr const char* specialized_symbol() { return ""; }
}

/// The si::system contains symbols for the dimensions, access to specialized
/// symbols for units, and information about the si locale facet.
struct system : system_tag {
    /// Unique id of this system
    static const long id;

    /// The symbols for each dimension
    static const char* kSymbol[];
    
    /// Obtain the symbol for a given dimension. The order matches the base_dimension enum.
    constexpr static const char* symbol_for(int i_dimension)
    {
        return kSymbol[i_dimension];
    }

    /// Obtain the specialized symbol for a given unit. 
    /// If no such symbol exists for U, this will return an empty string
    template<class U>
    static constexpr const char* specialized_symbol()
    {
        return symbol::specialized_symbol<U>();
    }
    
    /// Obtain the specialized symbol for a given dynamic_unit.
    /// If no such symbol exists for U, this will return an empty string
    static const char* specialized_symbol(dynamic_unit const& i_u);
    static void set_specialized_symbol(dynamic_unit const& i_u, char const* i_symbol);

    /// The dimensionless dynamic_unit for si
    using dimensionless_unit = unit<0, 0, 0, 0, 0, 0, 0, 0, system>;

    /// The facet type for si
    using facet = ::dim::si::facet;

    /// Obtain a pointer to a new si::facet. To avoid a memory leak, you must
    /// either delete this pointer or install it in a locale.
    static facet* make_default_facet() { return ::dim::si::make_default_facet(); }

    /// Install the facet in the global locale. Imbue all standard streams with
    /// this enhanced locale.
    static void install_facet(si::facet* i_fac = make_default_facet()) { ::dim::si::install_facet(i_fac); }
};

// clang-format off
// Fundamental dimensions and their associated unit types, quantity types, and symbols
DIM_DEFINE_QUANTITY_S(Length,      meter,     system, double, 1, 0, 0, 0, 0, 0, 0, 0, "m")
DIM_DEFINE_QUANTITY_S(Time,        second,    system, double, 0, 1, 0, 0, 0, 0, 0, 0, "s")
DIM_DEFINE_QUANTITY_S(Mass,        kilogram,  system, double, 0, 0, 1, 0, 0, 0, 0, 0, "kg")
DIM_DEFINE_QUANTITY_S(Angle,       radian,    system, double, 0, 0, 0, 1, 0, 0, 0, 0, "rad")
DIM_DEFINE_QUANTITY_S(Temperature, kelvin,    system, double, 0, 0, 0, 0, 1, 0, 0, 0, "K")
DIM_DEFINE_QUANTITY_S(Amount,      mole,      system, double, 0, 0, 0, 0, 0, 1, 0, 0, "mol")
DIM_DEFINE_QUANTITY_S(Current,     ampere,    system, double, 0, 0, 0, 0, 0, 0, 1, 0, "A")
DIM_DEFINE_QUANTITY_S(Luminosity,  candela,   system, double, 0, 0, 0, 0, 0, 0, 0, 1, "cd")

// Derived units and their associated quantity types, and symbols
DIM_DEFINE_QUANTITY_S(Frequency,           hertz,      system, double,  0, -1,  0,  0,  0,  0,  0,  0, "Hz")
DIM_DEFINE_QUANTITY_S(SolidAngle,          steradian,  system, double,  0,  0,  0,  2,  0,  0,  0,  0, "sr")
DIM_DEFINE_QUANTITY_S(Force,               newton,     system, double,  1, -2,  1,  0,  0,  0,  0,  0, "N")
DIM_DEFINE_QUANTITY_S(Pressure,            pascal,     system, double, -1, -2,  1,  0,  0,  0,  0,  0, "Pa")
DIM_DEFINE_QUANTITY_S(Energy,              joule,      system, double,  2, -2,  1,  0,  0,  0,  0,  0, "J")
DIM_DEFINE_QUANTITY_S(Power,               watt,       system, double,  2, -3,  1,  0,  0,  0,  0,  0, "W")
DIM_DEFINE_QUANTITY_S(Charge,              coulomb,    system, double,  0,  1,  0,  0,  0,  0,  1,  0, "C")
DIM_DEFINE_QUANTITY_S(Voltage,             volt,       system, double,  2, -3,  1,  0,  0,  0, -1,  0, "V")
DIM_DEFINE_QUANTITY_S(Capacitance,         farad,      system, double, -2,  4, -1,  0,  0,  0,  1,  0, "F")
DIM_DEFINE_QUANTITY_S(Resistance,          ohm,        system, double,  2, -3,  1,  0,  0,  0, -2,  0, "Ω")
DIM_DEFINE_QUANTITY_S(Conductance,         siemens,    system, double, -2,  3, -1,  0,  0,  0,  2,  0, "S")
DIM_DEFINE_QUANTITY_S(MagneticFlux,        weber,      system, double,  2, -2,  1,  0,  0,  0, -1,  0, "Wb")
DIM_DEFINE_QUANTITY_S(MagneticFluxDensity, tesla,      system, double,  0, -2,  1,  0,  0,  0, -1,  0, "T")
DIM_DEFINE_QUANTITY_S(Inductance,          henry,      system, double,  2, -2,  1,  0,  0,  0, -2,  0, "H")
DIM_DEFINE_QUANTITY_S(LuminousFlux,        lumen,      system, double,  0,  0,  0,  2,  0,  0,  0,  1, "Im")
DIM_DEFINE_QUANTITY_S(Luminance,           lux,        system, double, -2,  0,  0,  2,  0,  0,  0,  1, "Ix")
DIM_DEFINE_QUANTITY_S(CatalyticActivity,   katal,      system, double,  0, -1,  0,  0,  0,  1,  0,  0, "kat")
DIM_DEFINE_QUANTITY_S(Viscosity,           poiseuille, system, double, -1, -1,  1,  0,  0,  0,  0,  0, "Pl")

// Compound units without specialized unit names or symbols
DIM_DEFINE_QUANTITY(Area,                  meter2,  system, double, 2, 0, 0, 0, 0, 0, 0, 0)
DIM_DEFINE_QUANTITY(Volume,                meter3,  system, double, 3, 0, 0, 0, 0, 0, 0, 0)
constexpr unit_multiply_t<Time::unit, Time::unit> second2;
// clang-format on

// Compound quantities with specialized names but without specialized unit nmaes
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

// Alternate spelling
constexpr Length metre = meter;
constexpr decltype(meter_) metre_ = meter_;

// Common SI-allied units
constexpr Time minute = 60.0*second;
constexpr Time hour = 60.0*minute;
constexpr Area hectare = 1e4*meter2;
constexpr Volume liter = 1e-3*meter3;
constexpr Volume litre = liter;
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
constexpr Area acre = 43560.0 * foot * foot;
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
constexpr inline Temperature fahrenheit(double temp_f) { return (5./9.*temp_f + (273.15 - 5./9.*32.))*kelvin; }
constexpr inline double toFahrenheitValue(Temperature arg_t) { return (arg_t/rankine - 459.67); }

} // end of namespace si

// Some explicit template instantiations
extern template class dynamic_unit<si::system>;

} // end of namespace dim

#ifndef DIMENSION_NO_MATH
#include <cmath>
namespace dim
{
namespace si
{

/**********************************************************************
 * Trig function overloads inside dim::si
 *********************************************************************/

inline double sin(Angle const& q) { return ::std::sin(dimensionless_cast(q)); }
inline double cos(Angle const& q) { return ::std::cos(dimensionless_cast(q)); }
inline double tan(Angle const& q) { return ::std::tan(dimensionless_cast(q)); }
inline Angle asin(double const& x) { return ::std::asin(x)*radian; }
inline Angle acos(double const& x) { return ::std::acos(x)*radian; }
inline Angle atan(double const& x) { return ::std::atan(x)*radian; }
inline Angle atan2(double const& x, double const& y) { return ::std::atan2(x, y)*radian; }
template<class Q, DIM_IS_QUANTITY(Q)>
inline Angle atan2(Q const& x, Q const& y) { return ::std::atan2(dimensionless_cast(x), dimensionless_cast(y))*radian; }
}
}
#endif
