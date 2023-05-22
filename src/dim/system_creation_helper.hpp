#pragma once
/*
 * DEFINE_QUANTITY -- declare a new type for a given dimension.
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
#define DEFINE_QUANTITY(name, unit_name, System, Scalar, L, T, M, A, Te, Mol, Cur, Lum) \
    using name = ::dim::quantity<unit<L, T, M, A, Te, Mol, Cur, Lum, System>, Scalar>;  \
    constexpr name::unit unit_name##_;                                                  \
    constexpr name unit_name(1.0);

/*
 * Same as DEFINE_QUANTITY, but takes an additional char* argument at the end -- the symbol for output
 */
#define DEFINE_QUANTITY_S(name, unit_name, System, Scalar, L, T, M, A, Te, Mol, Cur, Lum, Symbol) \
    using name = ::dim::quantity<unit<L, T, M, A, Te, Mol, Cur, Lum, System>, Scalar>;            \
    constexpr name::unit unit_name##_;                                                            \
    constexpr name unit_name(1.0);                                                                \
    namespace symbol {                                                                            \
    template <>                                                                                   \
    constexpr const char* specialized_symbol<unit<L, T, M, A, Te, Mol, Cur, Lum, System>>()       \
    {                                                                                             \
        return Symbol;                                                                            \
    }                                                                                             \
    }

/// Convenience enum for accessing dynamic_quantity dimensions
struct base_dimension {
    enum types { Length = 0, Time, Mass, Angle, Temperature, Amount, Current, Luminosity };
};
