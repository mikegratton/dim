#pragma once
#include "definition.hpp"

namespace dim::si::literal {
/// Create literal formatters for both integer and floating point types
#define DIM_MAKE_LITERAL(symbol, unit)                                                                     \
    inline constexpr ::dim::quantity<decltype(unit)::type, double> operator"" symbol(long double x)        \
    {                                                                                                      \
        return static_cast<double>(x) * unit;                                                              \
    }                                                                                                      \
    inline constexpr ::dim::quantity<decltype(unit)::type, double> operator"" symbol(unsigned long long x) \
    {                                                                                                      \
        return static_cast<double>(x) * unit;                                                              \
    }

DIM_MAKE_LITERAL(_m, dim::si::meter_)
DIM_MAKE_LITERAL(_s, dim::si::second_)
DIM_MAKE_LITERAL(_kg, dim::si::kilogram_)
DIM_MAKE_LITERAL(_rad, dim::si::radian_)
DIM_MAKE_LITERAL(_K, dim::si::kelvin_)
DIM_MAKE_LITERAL(_mol, dim::si::mole_)
DIM_MAKE_LITERAL(_A, dim::si::ampere_)
DIM_MAKE_LITERAL(_cd, dim::si::candela_)

DIM_MAKE_LITERAL(_sr, dim::si::steradian_)
DIM_MAKE_LITERAL(_Hz, dim::si::hertz_)
DIM_MAKE_LITERAL(_N, dim::si::newton_)
DIM_MAKE_LITERAL(_Pa, dim::si::pascal_)
DIM_MAKE_LITERAL(_J, dim::si::joule_)
DIM_MAKE_LITERAL(_W, dim::si::watt_)
DIM_MAKE_LITERAL(_C, dim::si::coulomb_)
DIM_MAKE_LITERAL(_V, dim::si::volt_)
DIM_MAKE_LITERAL(_F, dim::si::farad_)
DIM_MAKE_LITERAL(_R, dim::si::ohm_)
DIM_MAKE_LITERAL(_S, dim::si::siemens_)
DIM_MAKE_LITERAL(_Wb, dim::si::weber_)
DIM_MAKE_LITERAL(_T, dim::si::tesla_)
DIM_MAKE_LITERAL(_H, dim::si::henry_)
DIM_MAKE_LITERAL(_Im, dim::si::lumen_)
DIM_MAKE_LITERAL(_Ix, dim::si::lux_)
DIM_MAKE_LITERAL(_kat, dim::si::katal_)

}  // namespace dim::si::literal
