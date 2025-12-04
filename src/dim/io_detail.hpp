#pragma once
#include "unit.hpp"
#include "dynamic_quantity.hpp"

namespace dim
{

/// Maximum length of unit symbol strings
constexpr int kMaxSymbol = 32;

namespace detail
{

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
char* print_exponentiated_symbol(char* o_symbol, char const* i_end, int8_t i_exponent, char const* i_symbol, bool& i_spaceit);

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
char* print_dimension(char* o_symbol, char const* i_end, int8_t i_exponent, int i_dimension, bool& i_spaceit)
{
    if (i_exponent == 0) {
        return o_symbol;
    }
    return print_exponentiated_symbol(o_symbol, i_end, i_exponent, System::symbol_for(i_dimension), i_spaceit);
}

/**
 * @brief Print a unit symbol to a given c-string buffer. The string will be null terminated, but may be 
 * truncated. You can detect truncation by checking if the return pointer equals the end pointer.
 * 
 * @param out_symbol On exit, the unit symbol string
 * @param end Pointer past the end of the out_symbol buffer. Output will be truncated to fit.
 * @param special_symbol If not null or empty, use this override as the symbol, copying it into out_symbol
 * @param unit The unit to print
 * @return Pointer just past the end of the output. This pointer points to a null character.
 */
template <class System, DIM_IS_SYSTEM(System)>
char* print_unit(char* o_symbol, char* i_end, char const* i_special_symbol, dynamic_unit<System> const& i_unit)
{
    if (i_special_symbol && *i_special_symbol) {
        while (*i_special_symbol) {
            *o_symbol++ = *i_special_symbol++;
        }
    } else {
        bool spaceit = false;
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.angle(),
                                                     static_cast<int>(base_dimension::Angle), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.mass(),
                                                     static_cast<int>(base_dimension::Mass), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.length(),
                                                     static_cast<int>(base_dimension::Length), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.temperature(),
                                                     static_cast<int>(base_dimension::Temperature), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.amount(),
                                                     static_cast<int>(base_dimension::Amount), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.current(),
                                                     static_cast<int>(base_dimension::Current), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.luminosity(),
                                                     static_cast<int>(base_dimension::Luminosity), spaceit);
        o_symbol = detail::print_dimension<System>(o_symbol, i_end, i_unit.time(),
                                                     static_cast<int>(base_dimension::Time), spaceit);
    }
    if (o_symbol < i_end) {
        *o_symbol = '\0';
    } else {
        *(i_end - 1) = '\0';
    }
    return o_symbol;
}

/**
 * @brief Parse a symbol consisting of only standard dimension symbols.
 *
 * @param symbol Symbol string to parse
 * @param end   Pointer past the end of symbol
 * @return A dynamic_quantity corresponding to the unit symbol. If the symbol
 * couldn't be parsed, this will be a bad_quantity().
 *
 * This base version just reports failure for all cases
 */
template <class Scalar, class System>
dynamic_quantity<Scalar, System> parse_standard_rep(char const*, char const*)
{
    return dynamic_quantity<Scalar, System>::bad_quantity();
}

/**
 * Parse unit_str to a dynamic_quantity. If the dynamic_unit doesn't match U's
 * dimensions, return false.  Otherwise "scale" represents the transform to
 * U's system.
 *
 * @param[out] scale Scale factor to transform input to system's units
 * @param[in] unit_str Unit string
 * @param[in] end Pointer past the end of unit string
 */
template <class U, class Scalar = double, DIM_IS_UNIT(U)>
bool parse_unit_dynamic(Scalar& i_scale, char const* i_unit_str, char const* i_end)
{
    dynamic_quantity<Scalar, typename U::system> dim = parse_standard_rep<Scalar, typename U::system>(i_unit_str, i_end);
    i_scale = dim.value();
    return (dim.unit() == index<U>());
}

/// Used in is_unit_char for determining which part of a string is a unit string
enum class unit_parse_state {
    kStart = 0,
    kSymbol = 1,
    kExponentStart = 2,
    kExponentSign = 3,
    kExponent = 4,
    kOperator = 0,
    kError = -2,
    kEnd = -1,
};

/**
 * @brief Stateful unit string scanner.
 *
 * Call accept() repeatedly until it returns false to find the (possible) bounds
 * of a unit string. This handles symbols, utf-8 (greek, degree, and angstrom),
 * parentheses, operators, and exponents. However, it is not the case that every
 * string accepted by the scanner is a valid unit string.
 *
 * This will terminate in kEnd if it is possible the unit string doesn't have an
 * error and in kError when the unit string is certainly malformed.
 *
 */
class unit_string_scanner
{
  public:
    unit_string_scanner()
        : m_state(unit_parse_state::kStart),
          m_utf_prev(0),
          m_parenthesis_depth(0),
          m_exponent_parenthesis(false)
    {
    }

    /**
     * Reset the scanner state to kStart
     */
    void reset()
    {
        m_state = unit_parse_state::kStart;
        m_utf_prev = 0;
        m_parenthesis_depth = 0;
        m_exponent_parenthesis = false;
    }

    /**
     * Start/continue scanning a string.
     * @param c Next character in string
     * @return True if c is likely part of the unit string
     */ 
    bool accept(char c);

    /**
     * Current state of the scanner
     */
    unit_parse_state state() const { return m_state; }

  private:
    bool issymbol(char c);
    unit_parse_state handle_symbol(char c);
    unit_parse_state handle_exponent(char c);
    unit_parse_state handle_exponent_start(char c);
    bool wrap_up();

    unit_parse_state m_state;
    unsigned char m_utf_prev;
    int m_parenthesis_depth;
    bool m_exponent_parenthesis;
};

/**
 * Is c a separator between a scalar and a unit string?
 * Valid values are '*', '_', and ' '.
 */
inline bool isseparator(char c)
{
    switch (c) {    
    case '_':
    case '*':
    case ' ':
        return true;
    default:
        return false;
    }
}

} // end of namespace detail
} // namespace dim

#ifdef DIM_EXCEPTIONS
#include "incommensurable_exception.hpp"

namespace dim
{
// This is defined here so that it has access to print_unit()
template <class S1, class S2>
std::string incommensurable_exception::print_error(dynamic_unit<S1> const& u1, dynamic_unit<S2> const& u2,
                                                   char const* i_message)
{
    std::string output(i_message);
    output += ": ";
    char buffer[256];
    print_unit(buffer, buffer + sizeof(buffer), u1);
    output += buffer;
    output += " != ";
    print_unit(buffer, buffer + sizeof(buffer), u2);
    output += buffer;
    return output;
}
} // namespace dim
#endif

// For compatibility, replicate from_chars_result for
// C++ < 17
#if __cplusplus < 201703L
#include <system_error>
namespace std
{
struct from_chars_result {
    char* ptr;
    errc ec;
};
} // namespace std
#endif