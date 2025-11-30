#pragma once
#include "dim/unit.hpp"
#include "dynamic_quantity.hpp"

namespace dim
{

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
    if (exponent == 0) {
        return out_symbol;
    }
    return print_exponentiated_symbol(out_symbol, end, exponent, System::symbol_for(dimension), spaceit);
}

template <class System, DIM_IS_SYSTEM(System)>
char* print_unit(char* out_symbol, char* end, char const* special_symbol, dynamic_unit<System> const& unit)
{
    if (special_symbol && *special_symbol) {
        while (*special_symbol) {
            *out_symbol++ = *special_symbol++;
        }
    } else {
        bool spaceit = false;
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.angle(),
                                                     static_cast<int>(base_dimension::Angle), spaceit);
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.mass(),
                                                     static_cast<int>(base_dimension::Mass), spaceit);
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
        out_symbol = detail::print_dimension<System>(out_symbol, end, unit.time(),
                                                     static_cast<int>(base_dimension::Time), spaceit);
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
 * @param end   -- Pointer past the end of symbol
 *
 * This base version just reports failure for all cases
 */
template <class Scalar, class System> dynamic_quantity<Scalar, System> parse_standard_rep(char const*, char const*)
{
    return dynamic_quantity<Scalar, System>::bad_quantity();
}

/**
 * Parse unit_str to a dynamic_quantity. If the dynamic_unit doesn't match U's
 * dimensions, return false.  Otherwise "scale" represents the transform to
 * U's system
 */
template <class U, class Scalar = double, DIM_IS_UNIT(U)>
bool parse_unit_dynamic(Scalar& scale, char const* unit_str, char const* end)
{
    dynamic_quantity<Scalar, typename U::system> dim = parse_standard_rep<Scalar, typename U::system>(unit_str, end);
    scale = dim.value();
    return (dim.unit() == index<U>());
}

/// Used in is_unit_char for determining which part of a string is a unit string
enum unit_parse_state {
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
        : m_state(kStart),
          m_utf_prev(0),
          m_parenthesis_depth(0),
          m_exponent_parenthesis(false)
    {
    }

    void reset()
    {
        m_state = kStart;
        m_utf_prev = 0;
        m_parenthesis_depth = 0;
        m_exponent_parenthesis = false;
    }

    bool accept(char c);

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
    case '.':
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
template <class S1, class S2>
std::string incommensurable_exception::print_error(dynamic_unit<S1> const& u1, dynamic_unit<S2> const& u2,
                                                   char const* message)
{
    std::string output(message);
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