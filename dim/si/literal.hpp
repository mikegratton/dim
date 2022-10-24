#pragma once
#include "definition.hpp"

namespace dim::si::literal
{

#define MAKE_LITERAL(symbol, unit) \
	inline constexpr ::dim::quantity<decltype(unit)::type, double> operator "" symbol(unsigned long long x) { return static_cast<double>(x)*unit; } \
	inline constexpr ::dim::quantity<decltype(unit)::type, double> operator "" symbol(long double x) { return static_cast<double>(x)*unit; }

MAKE_LITERAL(_m, dim::si::meter)
MAKE_LITERAL(_s, dim::si::second)
MAKE_LITERAL(_kg, dim::si::kilogram)
MAKE_LITERAL(_rad, dim::si::radian)
MAKE_LITERAL(_K, dim::si::kelvin)
MAKE_LITERAL(_mol, dim::si::mole)
MAKE_LITERAL(_A, dim::si::ampere)
MAKE_LITERAL(_cd, dim::si::candela)

MAKE_LITERAL(_sr, dim::si::steradian)
MAKE_LITERAL(_Hz, dim::si::hertz)
MAKE_LITERAL(_N, dim::si::newton)
MAKE_LITERAL(_Pa, dim::si::pascal)
MAKE_LITERAL(_J, dim::si::joule)
MAKE_LITERAL(_W, dim::si::watt)
MAKE_LITERAL(_C, dim::si::coulomb)
MAKE_LITERAL(_V, dim::si::volt)
MAKE_LITERAL(_F, dim::si::farad)
MAKE_LITERAL(_R, dim::si::ohm)
MAKE_LITERAL(_S, dim::si::siemens)
MAKE_LITERAL(_Wb, dim::si::weber)
MAKE_LITERAL(_T, dim::si::tesla)
MAKE_LITERAL(_H, dim::si::henry)
MAKE_LITERAL(_Im, dim::si::lumen)
MAKE_LITERAL(_Ix, dim::si::lux)
MAKE_LITERAL(_kat, dim::si::katal)
#undef MAKE_LITERAL
}
