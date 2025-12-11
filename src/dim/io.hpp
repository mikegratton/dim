#pragma once
#ifdef DIM_EXCEPTIONS
#include "incommensurable_exception.hpp"
#endif
#include "dynamic_quantity.hpp"
#include "io_detail.hpp"
#include <cstring>
#include <limits>
#include <system_error>
#if __cplusplus >= 201703L
#include <charconv>
#endif

namespace dim
{

/**
 * @brief A quantity represented as a scalar number and a symbol (char* string).
 */
template <class Scalar, DIM_IS_SCALAR(Scalar)>
class formatted_quantity
{
  public:
    /**
     * Create a formatted quantity. The symbol string is copied into an internal buffer if not null
     * @param i_value Scalar value
     * @param i_symbol unit symbol string
     */
    formatted_quantity(Scalar i_value = std::numeric_limits<Scalar>::quiet_NaN(), char const* i_symbol = "")
        : m_value(i_value),
          m_symbol{'\0'}
    {
        constexpr int kStop = kMaxSymbol - 1;
        if (i_symbol && *i_symbol) {
            int i = 0;
            for (; *i_symbol && i < kStop; i++) {
                m_symbol[i] = *i_symbol++;
            }
            m_symbol[i] = '\0';
        }
    }

    /**
     * A type indicating an incorrect format has occured.
     */
    static formatted_quantity bad_format()
    {
        return formatted_quantity(std::numeric_limits<Scalar>::quiet_NaN(), nullptr);
    }

    /**
     * Check if this is a bad format
     */
    bool is_bad() const { return isbad__(m_value); }

    Scalar const& value() const { return m_value; }

    Scalar& value() { return m_value; }

    void value(Scalar const& s) { m_value = s; }

    char const* symbol() const { return m_symbol; }

    char* symbol() { return m_symbol; }

  private:
    Scalar m_value;
    char m_symbol[kMaxSymbol];
};

/**
 * @brief Instructions on how to format type Q.
 *
 * Links a symbol to an affine transform. For input of a scalar s matching the
 * symbol, Q <- s*scale + add. For output, a formatted_quantity is produced via
 * formatted_quantity((q-add)/scale, symbol)
 *
 * Example: formatter("feet", foot) creats a formatter that can take a double
 * representing "ft" and transform it into a Length in the same system as
 * "foot", and it can take a Length l and format it as feet for output
 * regardless of the underlying rep.
 *
 * Formatters have the quantity type erased, tracking it at runtime via a unit_code.
 *
 */
template <class Scalar, class System>
class formatter
{
  public:
    using scalar = Scalar;
    using dynamic_type = dynamic_quantity<Scalar, System>;
    using formatted = formatted_quantity<scalar>;

    /**
     * Construct a new formatter.
     *
     * @param i_symbol Unit symbol string for this format
     * @param i_scale The scale in the affine transform from units of i_symbol
     * to the units of System
     * @param i_add The additive part of the affine transform. If this is a
     * bad_quantity, the additive part will be set to a zero quantity with units
     * matching i_scale.
     *
     * @throws (with DIM_EXCEPTIONS defined) If i_scale and i_add do not have matching units.
     */
    formatter(char const* i_symbol, dynamic_type const& i_scale, dynamic_type const& i_add = dynamic_type::bad_quantity())
        : m_scale(i_scale),
          m_add(i_add)
    {
        strncpy(m_symbol, i_symbol, kMaxSymbol - 1);
        if (m_add.is_bad()) {
            m_add = dynamic_type(0, m_scale.unit());
        }
        if (m_add.unit() != m_scale.unit()) {
#ifdef DIM_EXCEPTIONS
            throw incommensurable_exception(i_scale.unit(), i_add.unit(),
                                            "Units of affine transformation in formatter are incompatible");
#else
            m_add = dynamic_type::bad_quantity();
            m_scale = dynamic_type::bad_quantity();
            strncpy(m_symbol, "INCONSISTENT", kMaxSymbol - 1);
#endif
        }
    }

    /**
     * Construct a new formatter from an affine transform defined by static quantities
     *
     * @param i_symbol Unit symbol string for this format
     * @param i_scale The scale in the affine transform from units of i_symbol
     * to the units of System
     * @param i_add The additive part of the affine transform. 
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    formatter(char const* i_symbol, Q const& i_scale, Q const& i_add = Q(0))
        : formatter(i_symbol, dynamic_type(i_scale), dynamic_type(i_add))
    {
    }

    /**
     * Non-dimensionalize a quantity to match this format's units.
     * @param q Quantity to nondimensionalize
     * @return A scalar measuring q in units of this transform
     * @throws (with DIM_EXCEPTIONS defined) If Q is not compatible with this format's unit
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    scalar non_dim(Q const& q) const
    {
        return non_dim(dynamic_type(q));
    }

    /**
     * Non-dimensionalize a dynamic_quantity to match this format's units.
     * @param q Quantity to nondimensionalize
     * @return A scalar measuring q in units of this transform
     * @throws (with DIM_EXCEPTIONS defined) If Q is not compatible with this format's unit
     */
    scalar non_dim(dynamic_type const& q) const
    {
        dynamic_type result = (q + m_add) / m_scale;
#ifdef DIM_EXCEPTIONS
        if (result.dimensionless()) {
            return dimensionless_cast(result);
        }
        throw incommensurable_exception(::dim::index(q), m_scale.unit(),
                                        "Could not nondimensionalize quantity. Dimensions are incompatible");
#else
        return (result.dimensionless() ? dimensionless_cast(result) : std::numeric_limits<scalar>::quiet_NaN());
#endif
    }

    /**
     * Format a dynamic_quantity into a scalar and a unit symbol string.
     * @throws (with DIM_EXCEPTIONS defined) If q is not compatible with this format's unit
     */ 
    formatted output(dynamic_type const& q) const { return formatted(non_dim(q), symbol()); }

    /**
     * Format a quantity into a scalar and a unit symbol string.
     * @throws (with DIM_EXCEPTIONS defined) If Q is not compatible with this format's unit
     */ 
    template <class Q, DIM_IS_QUANTITY(Q)>
    formatted output(Q const& q) const
    {
        return formatted(non_dim(q), symbol());
    }

    /**
     * Transform a scalar value matching this unit symbol string into a quantity.
     * @throws (with DIM_EXCEPTIONS defined) If Q is not compatible with this format's unit
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q input(scalar const& s) const
    {
        return (s * m_scale + m_add).template as<Q>();
    }

    /**
     * Transform a scalar value matching this unit symbol string into a dynamic_quantity.
     */
    dynamic_type input(scalar const& s) const { return m_scale * dynamic_type(s) + m_add; }

    /**
     * Obtain the dynamic_unit index value for this formatter.
     */
    dynamic_unit<System> index() const { return m_scale.unit(); }

    /**
     * Inspect the unit symbol string.
     */ 
    char const* symbol() const { return m_symbol; }

  private:
    char m_symbol[kMaxSymbol];
    dynamic_type m_scale;
    dynamic_type m_add;
};


/**
* @brief Parse the string [i_begin, i_end) into a Scalar, storing the result in
* o_result.
*
* This can be specialized for particular Scalar types.
*/
template<class Scalar>
std::from_chars_result parse_scalar(Scalar& o_result, char const* i_begin, char const* i_end)
{
    std::from_chars_result result;
    #if __cplusplus >= 201703L
    result = std::from_chars(i_begin, i_end, o_result);
    #else
    o_result = static_cast<Scalar>(std::strtold(i_begin, &result.ptr));
    #endif
    return result;
}


/**
 * @brief Transform a region of characters into a formatted_quantity.
 *
 * Initial whitespace is ignored. The unit symbol string end is determined by
 * the unit_string_scanner. This can be whitespace, but also any character not
 * part of the symbol. E.g. "1.2_m^2]" would parse as {1.2, "m^2"} and return a
 * pointer to ']'.
 *
 * @param i_start Beginning of text region
 * @param i_end Pointer past the end of the text region
 * @param[out] o_formatted The formatted_quantity result.
 * @return a from_chars_result object consisting of a pointer past the end of
 * the last character processed and an error code. 
 *
 * Possible error codes:
 * * result_out_of_range -- scalar is too large to reprensent
 * * invalid_argument -- couldn't parse start of string to scalar or scanner
 *   detected unit string errors
 * * no_buffer_space -- symbol too long for symbol buffer
 * * no_message -- symbol is zero length
 *
 * If ec != std::errc{}, formatted is in an undefined state.
 */
template <class Scalar>
std::from_chars_result from_chars(char const* i_start, char const* i_end, formatted_quantity<Scalar>& o_formatted)
{
    Scalar s;
    o_formatted.symbol()[0] = '\0';
    
    std::from_chars_result result = parse_scalar(s, i_start, i_end);
    if (result.ptr == i_start) {
        if (result.ec == std::errc{}) {
            result.ec = std::errc::invalid_argument;
        }
        o_formatted = formatted_quantity<Scalar>::bad_format();
        return result;
    }
    o_formatted.value(s);
    if (detail::isseparator(*result.ptr)) {
        ++result.ptr;
    }
    char* cursor = o_formatted.symbol();
    char* symbolEnd = o_formatted.symbol() + kMaxSymbol;
    detail::unit_string_scanner scanner;
    while (cursor < symbolEnd && result.ptr < i_end && scanner.accept(*result.ptr)) {
        *cursor++ = *result.ptr++;
    }
    // Ensure the string is nul terminated
    if (cursor == symbolEnd) {
        result.ec = std::errc::no_buffer_space;
        o_formatted.symbol()[kMaxSymbol - 1] = '\0';
    } else {
        *cursor = '\0';
    }
    // Pack in scanner errors
    if (scanner.state() == detail::unit_parse_state::kError) {
        result.ec = std::errc::invalid_argument;
    }
    return result;
}


/**
 * Write a string representation "m/s^2" using the system type's facilities.
 * @return pointer past the last character written
 *
 * @param o_symbol Buffer to hold exponentiated symbol
 * @param i_end Pointer past the end of out_symbol
 * @param U The unit to print
 */
template <class U, DIM_IS_UNIT(U)>
char* print_unit(char* o_symbol, char* i_end, U const&)
{
    using System = typename U::system;
    return detail::print_unit(o_symbol, i_end, System::template specialized_symbol<U>(), dim::index<U>());
}

/**
 * Write a string representation like "m/s^2" using the system type's facilities.
 * @return pointer past the last character written
 *
 * @param o_symbol Buffer to hold exponentiated symbol
 * @param i_end Pointer past the end of out_symbol
 * @param i_unit The unit to print
 */
template <class System, DIM_IS_SYSTEM(System)>
char* print_unit(char* o_symbol, char* i_end, dynamic_unit<System> const& i_unit)
{
    return detail::print_unit(o_symbol, i_end, System::specialized_symbol(i_unit), i_unit);
}

/**
 * @brief Write a string representation of a quantity's unit. This version uses the system type.
 * @param o_unit_str Buffer to write to
 * @param i_end Pointer past the end of o_unit_str
 * @param q Quantity to use for template argument deduction
 * @return Pointer past the last character written
 */
template <class Q, DIM_IS_QUANTITY(Q)>
char* print_unit(char* o_unit_str, char* i_end, Q const&)
{
    return print_unit(o_unit_str, i_end, dim::index<Q>());
}

/**
 * @brief Write a string representation of a quantity's unit. This version uses the system type.
 * @param o_buf Buffer to write to
 * @param i_end Pointer past the end of buf
 * @param q Quantity whose unit to print
 * @return Pointer past the last character written
 */
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
char* print_unit(char* o_buf, char* i_end, DQ const& q)
{
    return print_unit(o_buf, i_end, q.unit());
}

} // end of namespace dim
