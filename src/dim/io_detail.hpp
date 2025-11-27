#pragma once
#include "dim/unit.hpp"
#include "dynamic_quantity.hpp"

namespace dim {

constexpr int kMaxSymbol = 32;

namespace detail {

/**
 * Print an 8 bit integer to a buffer without stdio.
 * @return Pointer past the last character written
 */
char* print_int8(char* o_buf, int8_t d);

/**
 * Print a symbol like "m^2".
 * @return pointer past the last character written
 * @note This does not null-terminate the string
 *
 * @param out_symbol Buffer to hold exponentiated symbol
 * @param end Pointer past the end of buffer
 * @param exponent The exponent. Nothing will be printed if this is zero
 * @param symbol The symbol
 * @param spaceit If true, insert _ before writing. The value will be set to true on exit.
 */ 
char* print_exponentiated_symbol(char* out_symbol, char const* end, int8_t exponent, char const* symbol, bool& spaceit);

/**
 * Print a symbol like "m^2".
 * @return pointer past the last character written
 * @note This does not null-terminate the string
 * @param out_symbol Buffer to hold exponentiated symbol
 * @param end Pointer past the end of buffer
 * @param exponent The exponent. Nothing will be printed if this is zero
 * @param dimension The dimension to print [0,7] in order of enum class base_dimension
 * @param spaceit If true, insert _ before writing. The value will be set to true on exit
 *                if exponent is not zero.
 */ 
template <class System>
char* print_dimension(char* out_symbol, char const* end, int8_t exponent, int dimension, bool& spaceit)
{
    if (exponent == 0) { return out_symbol; }
    return print_exponentiated_symbol(out_symbol, end, exponent, System::symbol_for(dimension), spaceit);    
}

template <class System, DIM_IS_SYSTEM(System)> char* print_unit(char* out_symbol, char* end, char const* special_symbol, dynamic_unit<System> const& unit)
{
    if (special_symbol && *special_symbol) {
        while (*special_symbol) {
            *out_symbol++ = *special_symbol++;
        }
    } else {
        bool spaceit = false;
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.angle(),
                                                     static_cast<int>(base_dimension::Angle), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.mass(), static_cast<int>(base_dimension::Mass),
                                                     spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.length(),
                                                     static_cast<int>(base_dimension::Length), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.temperature(),
                                                     static_cast<int>(base_dimension::Temperature), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.amount(),
                                                     static_cast<int>(base_dimension::Amount), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.current(),
                                                     static_cast<int>(base_dimension::Current), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.luminosity(),
                                                     static_cast<int>(base_dimension::Luminosity), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.time(), static_cast<int>(base_dimension::Time),
                                                     spaceit);
    }
    if (out_symbol < end) {
        *out_symbol = '\0';
    } else {
        *(end - 1) = '\0';
    }
    return out_symbol;
}

/**
 * @brief Parse a symbol consisting of only standard dimension symbols.
 *
 * @param symbol -- Symbol string to parse
 * @param nend   -- Size of symbol or -1 to indicate a null-terminated string
 *
 * This base version just reports failure for all cases
 */
template <class Scalar, class System>
dynamic_quantity<Scalar, System> parse_standard_rep(char const*, int)
{
    return dynamic_quantity<Scalar, System>::bad_quantity();
}

/**
 * @brief Parse a symbol consisting of only standard dimension symbols.
 *
 * @param symbol -- Symbol string to parse, in a null-terminated string 
 */
template <class Scalar, class System>
dynamic_quantity<Scalar, System> parse_standard_rep(const char* symbol)
{
    return parse_standard_rep<Scalar, System>(symbol, -1);
}

/**
 * Parse unit_str to a dynamic_quantity. If the dynamic_unit doesn't match U's
 * dimensions, return false.  Otherwise "scale" represents the transform to
 * U's system
 */
template <class U, class Scalar = double, DIM_IS_UNIT(U)>
bool parse_unit_dynamic(Scalar& scale, char const* unit_str, char const* end)
{
    dynamic_quantity<Scalar, typename U::system> dim = parse_standard_rep<Scalar, typename U::system>(unit_str, end - unit_str);
    scale = dim.value();
    return (dim.unit() == index<U>());
}

/**
* Return a pointer to the first unit character in unit_string. This skips any
* number/combination of " ", "_", and "*".
*/
char const* advance_past_separator(char const* unit_string, char const* end);

enum unit_parse_state
{
    kUnit,
    kExponent,
    
};
bool is_unit_char(char c, unit_parse_state& io_state);

bool is_separator_character(char c);

}  // end of namespace detail

}  // end of namespace dim

// For compatibility, replicate from_chars_result for
// C++ < 17
#if __cplusplus < 201703L
#include <system_error>
namespace std
{
struct from_chars_result 
{
    char* ptr;
    errc ec;
};
}
#endif