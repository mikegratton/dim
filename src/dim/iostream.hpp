#pragma once
#include <iostream>
#include "dim/io_detail.hpp"
#include "dynamic_quantity.hpp"
#include "format_map.hpp"
#include "dim/io.hpp"
#include "facet.hpp"

namespace dim
{

namespace detail {

template<class Scalar>
bool extract_formatted_quantity(formatted_quantity<Scalar>& formatted, std::istream& is)
{
    std::size_t i = 0;
    char* cursor = formatted.symbol();
    is >> formatted.value();    
    while (is && detail::isseparator(is.peek())) { is.ignore(); }
    detail::unit_parse_state state = detail::kStart;
    char* end = formatted.symbol() + kMaxSymbol;
    detail::unit_string_scanner scanner;
    while (is && scanner.accept(is.peek()) && cursor < end) { is.get(*cursor++); }
    if (cursor < end) { 
        *cursor = '\0';
    } else {
        *(end-1) = '\0';
    }
    return !(formatted.is_bad() || scanner.state() == kError || formatted.symbol() == cursor);
}
}  // namespace detail

/// Write a formatted quantity to a stream.
template <class Scalar, DIM_IS_SCALAR(Scalar)> inline std::ostream& operator<<(std::ostream& os, formatted_quantity<Scalar> const& fq)
{
    return os << fq.value() << "_" << fq.symbol();
}

/// Write a quantity Q to a stream using the facet.
template <class Q, DIM_IS_QUANTITY(Q)> std::ostream& operator<<(std::ostream& os, Q const& q)
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
template <class Scalar, class System, DIM_IS_SCALAR(Scalar), DIM_IS_SYSTEM(System)>
std::ostream& operator<<(std::ostream& os, dynamic_quantity<Scalar, System> const& dq)
{
    using facet = typename System::facet;
    formatted_quantity<Scalar> f;
    if (std::has_facet<facet>(os.getloc())) {
        f = std::use_facet<facet>(os.getloc()).format(dq);
    } else {        
        format_quantity(f, dq);        
    }
    return os << f;
}

/// Write the unit U to a stream using the default output symbol
template <class U, DIM_IS_UNIT(U)> std::ostream& operator<<(std::ostream& os, U const& u)
{
    char buf[128];
    return os << print_unit(buf, buf + sizeof(buf), u);
}

/// Write the dynamic_unit to a stream using the default output symbol
template <class System, DIM_IS_SYSTEM(System)> std::ostream& operator<<(std::ostream& os, dynamic_unit<System> const& u)
{
    char buf[128];
    return os << print_unit(buf, buf + sizeof(buf), u);    
}

/**
 * @breif Calls parse_quantity, using the quantity_facet to extract a custom unit map if available.
 */
template <class Q, DIM_IS_QUANTITY(Q)> std::istream& operator>>(std::istream& is, Q& out_q)
{
    using Scalar = typename Q::scalar;
    using facet = typename Q::system::facet;
    formatted_quantity<Scalar> formatted;        
    if (detail::extract_formatted_quantity(formatted, is)) {        
        if (std::has_facet<facet>(is.getloc())) {            
            out_q = std::use_facet<facet>(is.getloc()).template format<Q>(formatted);
            if (out_q.is_bad()) {
                is.setstate(std::ios_base::failbit);
            }
        } else if (!parse_quantity<Q>(out_q, formatted)) {
            is.setstate(std::ios_base::failbit);
        }
    } else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

/// Use the facet to do formatted input for a dynamic quantity
template <class Scalar, class System> std::istream& operator>>(std::istream& is, dynamic_quantity<Scalar, system_tag>& o_q)
{
    using facet = typename System::facet;
    Scalar value;
    formatted_quantity<Scalar> formatted;        
    if (detail::extract_formatted_quantity(formatted, is)) {        
        if (std::has_facet<facet>(is.getloc())) {            
            o_q = std::use_facet<facet>(is.getloc()).format(formatted);
            if (o_q.is_bad()) {
                is.setstate(std::ios_base::failbit);
            }
        } else if (!parse_quantity(o_q, formatted)) {
            is.setstate(std::ios_base::failbit);
        }
    } else {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}


} // end of namespace dim
