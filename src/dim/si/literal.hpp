#pragma once
#include "definition.hpp"

namespace dim::si::literal
{

#define MAKE_LITERAL(symbol, unit) \
    inline constexpr ::dim::quantity<decltype(unit)::type, double> operator "" symbol(long double x) { return static_cast<double>(x)*unit; } \
    inline constexpr ::dim::quantity<decltype(unit)::type, double> operator "" symbol(unsigned long long x) { return static_cast<double>(x)*unit; } 




MAKE_LITERAL(_m, dim::si::meter_)
MAKE_LITERAL(_s, dim::si::second_)
MAKE_LITERAL(_kg, dim::si::kilogram_)
MAKE_LITERAL(_rad, dim::si::radian_)
MAKE_LITERAL(_K, dim::si::kelvin_)
MAKE_LITERAL(_mol, dim::si::mole_)
MAKE_LITERAL(_A, dim::si::ampere_)
MAKE_LITERAL(_cd, dim::si::candela_)

MAKE_LITERAL(_sr, dim::si::steradian_)
MAKE_LITERAL(_Hz, dim::si::hertz_)
MAKE_LITERAL(_N, dim::si::newton_)
MAKE_LITERAL(_Pa, dim::si::pascal_)
MAKE_LITERAL(_J, dim::si::joule_)
MAKE_LITERAL(_W, dim::si::watt_)
MAKE_LITERAL(_C, dim::si::coulomb_)
MAKE_LITERAL(_V, dim::si::volt_)
MAKE_LITERAL(_F, dim::si::farad_)
MAKE_LITERAL(_R, dim::si::ohm_)
MAKE_LITERAL(_S, dim::si::siemens_)
MAKE_LITERAL(_Wb, dim::si::weber_)
MAKE_LITERAL(_T, dim::si::tesla_)
MAKE_LITERAL(_H, dim::si::henry_)
MAKE_LITERAL(_Im, dim::si::lumen_)
MAKE_LITERAL(_Ix, dim::si::lux_)
MAKE_LITERAL(_kat, dim::si::katal_)
#undef MAKE_LITERAL
}
