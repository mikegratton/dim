#pragma once
#include <cstdio>
#include <cstring>

#include "dynamic_quantity.hpp"

namespace dim {

constexpr int kMaxSymbol = 32;

namespace detail {

template <class System>
void print_helper(char** out_symbol, int a, int dimension, bool& space)
{
    if (a == 0) return;
    if (space) *(*out_symbol)++ = '_';
    int advance;
    if (a == 1)
        advance = sprintf(*out_symbol, "%s", System::symbol_for(dimension));
    else
        advance = sprintf(*out_symbol, "%s^%d", System::symbol_for(dimension), a);
    (*out_symbol) = *out_symbol + advance;
    space = true;
}

template <class U, DIM_IS_UNIT(U)>
void print_unit(char* out_symbol, U const&, bool& spaceit)
{
    using System = typename U::system;
    const char* sym = System::template specialized_symbol<U>();
    if (sym[0]) {
        if (spaceit)
            sprintf(out_symbol, "_%s", sym);
        else
            sprintf(out_symbol, "%s", sym);
        return;
    }
    char** tmp_symbol = &out_symbol;
    print_helper<System>(tmp_symbol, U::angle(),       static_cast<int>(base_dimension::Angle), spaceit);
    print_helper<System>(tmp_symbol, U::mass(),        static_cast<int>(base_dimension::Mass), spaceit);
    print_helper<System>(tmp_symbol, U::length(),      static_cast<int>(base_dimension::Length), spaceit);
    print_helper<System>(tmp_symbol, U::temperature(), static_cast<int>(base_dimension::Temperature), spaceit);
    print_helper<System>(tmp_symbol, U::amount(),      static_cast<int>(base_dimension::Amount), spaceit);
    print_helper<System>(tmp_symbol, U::current(),     static_cast<int>(base_dimension::Current), spaceit);
    print_helper<System>(tmp_symbol, U::luminosity(),  static_cast<int>(base_dimension::Luminosity), spaceit);
    print_helper<System>(tmp_symbol, U::time(),        static_cast<int>(base_dimension::Time), spaceit);
}


/**
 * @brief Print a dynamic unit (specialized by each system)
 */
template <class System, DIM_IS_SYSTEM(System)>
void print_unit(char* buf, dynamic_unit<System> const& unit, bool spaceit)
{
    if (spaceit) { buf += sprintf(buf, "_"); }
    sprintf(buf, "[%d %d %d %d %d %d %d %d]", 
        static_cast<int>(unit.get(0)), 
        static_cast<int>(unit.get(1)), 
        static_cast<int>(unit.get(2)), 
        static_cast<int>(unit.get(3)), 
        static_cast<int>(unit.get(4)),
        static_cast<int>(unit.get(5)), 
        static_cast<int>(unit.get(6)), 
        static_cast<int>(unit.get(7)));
}

char const* advance_past_separator(char const* unit_string);

/**
 * @brief Parse a symbol consisting of only standard dimension symbols.
 *
 * @param symbol -- Symbol string to parse
 * @param nend   -- Size of symbol or -1 to indicate a null-terminated string
 *
 * This base version just reports failure for all cases
 */
template <class Scalar, class System>
dynamic_quantity<Scalar, System> parse_standard_rep(const char*, int)
{
    return dynamic_quantity<Scalar, System>::bad_quantity();
}

template <class Scalar, class System>
dynamic_quantity<Scalar, System> parse_standard_rep(const char* symbol)
{
    return parse_standard_rep<Scalar, System>(symbol, -1);
}

/*
 * Parse unit_str to a dynamic_quantity. If the dynamic_unit doesn't match U's
 * dimensions, return false.  Otherwise "scale" represents the transform to
 * U's system
 */
template <class U, class Scalar = double, DIM_IS_UNIT(U)>
bool parse_unit_dynamic(Scalar& scale, const char* unit_str)
{
    dynamic_quantity<Scalar, typename U::system> dim = parse_standard_rep<Scalar, typename U::system>(unit_str);
    scale = dim.value();
    return (dim.unit() == index<U>());
}

// Tools to examine a string, separating the floating point parts.
enum char_parse_state { kStart, kSignificand, kFraction, kExponentStart, kExponent, kUnit, kError };
bool is_unit_char(char c, char_parse_state& io_state);

bool is_float_part(char c, char_parse_state& io_state);

}  // end of namespace detail

}  // end of namespace dim
