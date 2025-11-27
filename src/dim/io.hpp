#pragma once
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
template <class scalar, DIM_IS_SCALAR(scalar)> class formatted_quantity
{
  public:
    formatted_quantity(char const* symbol_ = nullptr, scalar value_ = std::numeric_limits<scalar>::quiet_NaN())
        : m_value(value_),
          m_symbol{'\0'}
    {
        constexpr int kStop = kMaxSymbol - 1;
        if (symbol_) {
            int i = 0;
            for (; *symbol_ && i < kStop; i++) {
                m_symbol[i] = *symbol_++;
            }
            m_symbol[i] = '\0';
        }
    }

    static formatted_quantity bad_format()
    {
        return formatted_quantity(nullptr, std::numeric_limits<scalar>::quiet_NaN());
    }

    bool is_bad() const { return isbad__(m_value); }

    scalar const& value() const { return m_value; } 

    scalar& value() { return m_value; } 

    void value(scalar const& s) { m_value = s; }

    char const* symbol() const { return m_symbol; }

    char* symbol() { return m_symbol; }

    char* set_symbol() { return m_symbol; }

  private:
    scalar m_value;
    char m_symbol[kMaxSymbol];
};

/**
 * @brief Instructions on how to format type Q.
 *
 * Links a symbol to an affine transform. For input of a scalar s matching the
 * symbol, Q <- s*scale + add. For output, a formatted_quantity is produced via
 * formatted_quantity(symbol, (q-add)/scale)
 *
 * Example: formatter("feet", foot) creats a formatter that can take a double
 * representing "ft" and transform it into a Length in the same system as
 * "foot", and it can take a Length l and format it as feet for output
 * regardless of the underlying rep.
 *
 * Formatters have the quantity type erased, tracking it at runtime via a unit_code.
 *
 */
template <class S, class System> class formatter
{
  public:
    using scalar = S;
    using dynamic_type = dynamic_quantity<S, System>;
    using formatted = formatted_quantity<scalar>;

    formatter(char const* symbol_, dynamic_type const& scale_, dynamic_type const& add_ = dynamic_type::bad_quantity())
        : scale(scale_),
          add(add_)
    {
        strncpy(m_symbol, symbol_, kMaxSymbol - 1);
        if (add.is_bad()) {
            add = dynamic_type(0, scale.unit());
        }
        if (add.unit() != scale.unit()) {
            add = dynamic_type::bad_quantity();
            scale = dynamic_type::bad_quantity();
            strncpy(m_symbol, "INCONSISTENT", kMaxSymbol - 1);
        }
    }

    template <class Q, DIM_IS_QUANTITY(Q)>
    formatter(char const* symbol_, Q const& scale_, Q const& add_ = Q(0))
        : formatter(symbol_, dynamic_type(scale_), dynamic_type(add_))
    {
    }

    template <class Q, DIM_IS_QUANTITY(Q)> scalar non_dim(Q const& q) const { return non_dim(dynamic_type(q)); }

    scalar non_dim(dynamic_type const& q) const
    {
        dynamic_type result = (q + add) / scale;
        return (result.dimensionless() ? dimensionless_cast(result) : std::numeric_limits<scalar>::quiet_NaN());
    }

    formatted output(dynamic_type const& q) const { return formatted(symbol(), non_dim(q)); }

    template <class Q, DIM_IS_QUANTITY(Q)> formatted output(Q const& q) const
    {
        return formatted(symbol(), non_dim(q));
    }

    template <class Q, DIM_IS_QUANTITY(Q)> Q input(scalar const& s) const { return (s * scale + add).template as<Q>(); }

    dynamic_type input(scalar const& s) const { return scale * dynamic_type(s) + add; }

    dynamic_unit<System> index() const { return scale.unit(); }

    char const* symbol() const { return m_symbol; }

  private:
    char m_symbol[kMaxSymbol];
    dynamic_type scale;
    dynamic_type add;
};

/**
 * Print a unit string like "m/s^2" using the system
 * @return pointer past the last character written
 *
 * @param out_symbol Buffer to hold exponentiated symbol
 * @param end Pointer past the end of out_symbol
 * @param U The unit to print
 */
template <class U, DIM_IS_UNIT(U)> char* print_unit(char* out_symbol, char* end, U const&)
{
    using System = typename U::system;
    return detail::print_unit(out_symbol, end, System::template specialized_symbol<U>(), dim::index<U>());
}

/**
 * Print a unit string like "m/s^2" using the system
 * @return pointer past the last character written
 *
 * @param out_symbol Buffer to hold exponentiated symbol
 * @param end Pointer past the end of out_symbol
 * @param unit The unit to print
 */
template <class System, DIM_IS_SYSTEM(System)>
char* print_unit(char* out_symbol, char* end, dynamic_unit<System> const& unit)
{
    return detail::print_unit(out_symbol, end, System::specialized_symbol(unit), unit);    
}

/**
 * @brief Basic formatting of a quantity's unit according to the system
 * @param o_unit_str Buffer to write to
 * @param end Pointer past the end of o_unit_str
 * @param q Quantity to use for template argument deduction
 * @return Pointer past the last character written
 */
template <class Q, DIM_IS_QUANTITY(Q)> char* print_unit(char* o_unit_str, char* end, Q const&)
{
    return print_unit(o_unit_str, end, dim::index<Q>());
}

/**
 * @brief Basic formatting of a dynamic_unit according to the system
 * @param buf Buffer to write to
 * @param end Pointer past the end of buf
 * @param q Quantity whose unit to print
 * @return Pointer past the last character written
 */
template <class Scalar, class System, DIM_IS_SYSTEM(System)>
char* print_unit(char* buf, char* end, dynamic_quantity<Scalar, System> const& q)
{
    return print_unit(buf, end, q.unit());
}

/**
 * @brief Transform a region of characters into a formatted_quantity.
 *
 * result_out_of_range -- scalar is too large to reprensent
 * invalid_argument -- couldn't parse start of string to scalar
 * no_buffer_space -- symbol too long for symbol buffer
 * no_message -- symbol is zero length
 */
template <class Scalar>
std::from_chars_result from_chars(char const* start, char const* end, formatted_quantity<Scalar>& formatted)
{
    Scalar s;
    formatted.symbol()[0] = '\0';
    std::from_chars_result result;
#if __cplusplus >= 201703L
    result = std::from_chars(start, end, s);
#else
    s = static_cast<Scalar>(std::strtold(start, &result.ptr));
#endif
    if (result.ptr == start) {
        if (result.ec == std::errc{}) {
            result.ec = std::errc::invalid_argument;
        }
        formatted = formatted_quantity<Scalar>::bad_format();
        return result;
    }
    formatted.value(s);
    result.ptr = detail::advance_past_separator(result.ptr, end);
    char* cursor = formatted.symbol();
    char* symbolEnd = formatted.symbol() + kMaxSymbol;
    detail::unit_parse_state parse_state = detail::kUnit;
    while (cursor < symbolEnd && result.ptr < end && detail::is_unit_char(*result.ptr, parse_state)) {
        *cursor++ = *result.ptr++;
    }
    if (cursor < symbolEnd) {
        *cursor = '\0';
    } else {
        result.ec = std::errc::no_buffer_space;
        formatted.symbol()[kMaxSymbol - 1] = '\0';
    }
    if (cursor == formatted.symbol() && result.ec == std::errc{}) {
        result.ec = std::errc::no_message;
    }
    return result;
}

} // end of namespace dim
