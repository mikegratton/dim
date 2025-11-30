#pragma once
/**
 * DIM_DEFINE_QUANTITY -- declare a new type for a given dimension.
 *
 * INPUT:
 * 	name -- the name of the new type (e.g. "Length")
 * 	unit_name -- the name of its units (e.g. "meter")
 * 	System -- the system it belongs to (e.g. "si__")
 * 	L -- length dimension (integer)
 * 	T -- time dimension (integer)
 * 	M -- mass dimension (integer)
 * 	A -- angle dimension (integer)
 * 	Te -- temperature dimension (integer)
 * 	Mol -- molar amount dimension (integer)
 * 	Cur -- electric current dimension (integer)
 * 	Lum -- luminosity dimension (integer)
 *
 * OUTPUT:
 *   Declares a quantity type "name"
 *   Creates a constexpr for the units of "unit_name_"
 *   Creates a constexpr for the quantity of "unit_name"
 */
#define DIM_DEFINE_QUANTITY(name, unit_name, System, Scalar, L, T, M, A, Te, Mol, Cur, Lum)                            \
    using name = ::dim::quantity<unit<L, T, M, A, Te, Mol, Cur, Lum, System>, Scalar>;                                 \
    constexpr name::unit unit_name##_;                                                                                 \
    constexpr name unit_name(1.0);

/**
 * DIM_DEFINE_QUANTITY_S -- declare a new type for a given dimension with a specialized output symbol
 *
 * INPUT:
 * 	name -- the name of the new type (e.g. "Length")
 * 	unit_name -- the name of its units (e.g. "meter")
 * 	System -- the system it belongs to (e.g. "si__")
 * 	L -- length dimension (integer)
 * 	T -- time dimension (integer)
 * 	M -- mass dimension (integer)
 * 	A -- angle dimension (integer)
 * 	Te -- temperature dimension (integer)
 * 	Mol -- molar amount dimension (integer)
 * 	Cur -- electric current dimension (integer)
 * 	Lum -- luminosity dimension (integer)
 *  Symbol -- Character string with the specialized symbol (e.g. "V")
 *
 * OUTPUT:
 *   Declares a quantity type "name"
 *   Creates a constexpr for the units of "unit_name_"
 *   Creates a constexpr for the quantity of "unit_name"
 *   Adds a specialization of specialized_symbol<> so that the default output symbol is Symbol
 */
#define DIM_DEFINE_QUANTITY_S(name, unit_name, System, Scalar, L, T, M, A, Te, Mol, Cur, Lum, Symbol)                  \
    using name = ::dim::quantity<unit<L, T, M, A, Te, Mol, Cur, Lum, System>, Scalar>;                                 \
    constexpr name::unit unit_name##_;                                                                                 \
    constexpr name unit_name(1.0);                                                                                     \
    namespace symbol                                                                                                   \
    {                                                                                                                  \
    template <> constexpr const char* specialized_symbol<unit<L, T, M, A, Te, Mol, Cur, Lum, System>>()                \
    {                                                                                                                  \
        return Symbol;                                                                                                 \
    }                                                                                                                  \
    }

/**
 * Create literal formatters for both integer and floating point types.
 * @param symbol -- the suffix to use e.g. "m" for 1.0_m
 * @param unit -- A unit or quantity type to use for formatting, e.g. si::meter_ or si::meter
 */
#define DIM_MAKE_LITERAL(symbol, unit)                                                                                 \
    inline constexpr decltype(1.0 * (unit)) operator""_##symbol(long double x)                                         \
    {                                                                                                                  \
        return static_cast<double>(x) * (unit);                                                                        \
    }                                                                                                                  \
    inline constexpr decltype(1.0 * (unit)) operator""_##symbol(unsigned long long x)                                  \
    {                                                                                                                  \
        return static_cast<double>(x) * (unit);                                                                        \
    }
