#pragma once
#include <iostream>
#include "facet.hpp"

namespace dim
{

namespace detail
{

/**
 * @brief Extract a formatter_quantity from an istream.
 *
 * This uses operator>> to extract the scalar part, followed by the
 * `unit_string_scanner` to take characters from the stream until it determines
 * the unit symbol string is finished.
 *
 * @return False if: (a) the scalar is bad, (b) the scanner ends in an error
 * state, or (c) the stream reports failure.
 */
template <class Scalar>
bool extract_formatted_quantity(formatted_quantity<Scalar>& o_formatted, std::istream& is)
{
    std::size_t i = 0;
    char* cursor = o_formatted.symbol();
    is >> o_formatted.value();
    while (is && detail::isseparator(is.peek())) {
        is.ignore();
    }    
    char* end = o_formatted.symbol() + kMaxSymbol;
    detail::unit_string_scanner scanner;
    while (is && scanner.accept(is.peek()) && cursor < end) {
        is.get(*cursor++);
    }
    if (cursor < end) {
        *cursor = '\0';
    } else {
        *(end - 1) = '\0';
    }
    return !(o_formatted.is_bad() || scanner.state() == detail::unit_parse_state::kError || is.fail());
}
} // namespace detail

/// Write a formatted quantity to a stream.
template <class Scalar, DIM_IS_SCALAR(Scalar)>
inline std::ostream& operator<<(std::ostream& os, formatted_quantity<Scalar> const& i_formatted)
{
    return os << i_formatted.value() << "_" << i_formatted.symbol();
}

/// Write a quantity Q to a stream using the facet.
template <class Q, DIM_IS_QUANTITY(Q)>
std::ostream& operator<<(std::ostream& os, Q const& q)
{
    using facet = typename Q::system::facet;
    formatted_quantity<typename Q::scalar> f;
    if (std::has_facet<facet>(os.getloc())) {
        f = std::use_facet<facet>(os.getloc()).template format<Q>(q);
    } else {
        format_quantity(f, q);
    }
    return os << f;
}

/// Write a dynamic_quantity to a stream using the facet.
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
std::ostream& operator<<(std::ostream& os, DQ const& dq)
{
    using facet = typename DQ::system::facet;
    formatted_quantity<typename DQ::scalar> f;
    if (std::has_facet<facet>(os.getloc())) {
        f = std::use_facet<facet>(os.getloc()).format(dq);
    } else {
        format_quantity(f, dq);
    }
    return os << f;
}

/// Write the unit U to a stream using the default output symbol
template <class U, DIM_IS_UNIT(U)>
std::ostream& operator<<(std::ostream& os, U const& u)
{
    char buf[128];
    return os << print_unit(buf, buf + sizeof(buf), u);
}

/// Write the dynamic_unit to a stream using the default output symbol
template <class System, DIM_IS_SYSTEM(System)>
std::ostream& operator<<(std::ostream& os, dynamic_unit<System> const& u)
{
    char buf[128];
    return os << print_unit(buf, buf + sizeof(buf), u);
}

/**
 * @brief Extract a quantity from the stream using the facet if available.
 * @throws (If DIM_EXCEPTIONS is defined) if the extracted quantity has
 * different dimensions than Q.
 */
template <class Q, DIM_IS_QUANTITY(Q)>
std::istream& operator>>(std::istream& is, Q& o_quantity)
{
    using Scalar = typename Q::scalar;
    using facet = typename Q::system::facet;
    formatted_quantity<Scalar> formatted;
    if (detail::extract_formatted_quantity(formatted, is)) {
        if (std::has_facet<facet>(is.getloc())) {
            o_quantity = std::use_facet<facet>(is.getloc()).template format<Q>(formatted);
            if (o_quantity.is_bad()) {
                is.setstate(std::ios_base::failbit);
            }
        } else if (!parse_quantity<Q>(o_quantity, formatted)) {
            is.setstate(std::ios_base::failbit);
        }
    } else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

/**
 * @brief Extract a dynamic_quantity from the stream using the facet if available.
 */
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
std::istream& operator>>(std::istream& is, DQ& o_quantity)
{
    using facet = typename DQ::system::facet;
    using scalar = typename DQ::scalar;
    scalar value;
    formatted_quantity<scalar> formatted;
    if (detail::extract_formatted_quantity(formatted, is)) {
        if (std::has_facet<facet>(is.getloc())) {
            o_quantity = std::use_facet<facet>(is.getloc()).format(formatted);
            if (o_quantity.is_bad()) {
                is.setstate(std::ios_base::failbit);
            }
        } else if (!parse_quantity(o_quantity, formatted)) {
            is.setstate(std::ios_base::failbit);
        }
    } else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

} // end of namespace dim
