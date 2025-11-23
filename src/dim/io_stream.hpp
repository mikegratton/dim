#pragma once
#include <iostream>
#include "dim/dynamic_quantity.hpp"
#include "quantity_facet.hpp"

namespace dim
{

/// Write a formatted quantity to a stream
template <class Scalar, DIM_IS_SCALAR(Scalar)> inline std::ostream& operator<<(std::ostream& os, formatted_quantity<Scalar> const& fq)
{
    return os << fq.value() << "_" << fq.symbol();
}

/// Write a quantity Q to a stream using the facet
template <class Q, DIM_IS_QUANTITY(Q)> std::ostream& operator<<(std::ostream& os, Q const& q)
{
    using facet = typename Q::system::facet;
    if (std::has_facet<facet>(os.getloc())) {
        os << std::use_facet<facet>(os.getloc()).template format<Q>(q);
    } else {
        os << dimensionless_cast(q) << "_" << typename Q::unit();
    }
    return os;
}

/// Write a dynamic_quantity to a stream using the facet
template <class Scalar, class System, DIM_IS_SCALAR(Scalar), DIM_IS_SYSTEM(System)>
std::ostream& operator<<(std::ostream& os, dynamic_quantity<Scalar, System> const& dq)
{
    using facet = typename System::facet;
    if (std::has_facet<facet>(os.getloc())) {
        os << std::use_facet<facet>(os.getloc()).format(dq);
    } else {
        os << dimensionless_cast(dq) << "_" << dq.unit();
    }
    return os;
}

namespace detail
{
bool extract_quantity(char* o_buf, char*& o_unit_start, std::size_t bufmax, std::istream& is);
};

/// Write the unit U to a stream using the default output symbol
template <class U, DIM_IS_UNIT(U)> std::ostream& operator<<(std::ostream& os, U const& u)
{
    bool spaceit = false;
    char buf[128];
    detail::print_unit<U>(buf, u, spaceit);
    os << buf;
    return os;
}

/// Write the dynamic_unit to a stream using the default output symbol
template <class System, DIM_IS_SYSTEM(System)> std::ostream& operator<<(std::ostream& os, dynamic_unit<System> const& u)
{
    bool spaceit = false;
    char buf[128];
    detail::print_unit(buf, u, spaceit);
    os << buf;
    return os;
}

/**
 * @breif Calls parse_quantity, using the quantity_facet to extract a custom unit map if available.
 */
template <class Q, DIM_IS_QUANTITY(Q)> std::istream& operator>>(std::istream& is, Q& out_q)
{
    using Scalar = typename Q::scalar;
    using facet = typename Q::system::facet;
    char quanity_string[128];
    char* unit;
    if (detail::extract_quantity(quanity_string, unit, sizeof(quanity_string), is)) {
        long double s = std::strtold(quanity_string, nullptr);
        Scalar value(s);
        if (std::has_facet<facet>(is.getloc())) {
            out_q = std::use_facet<facet>(is.getloc()).template format<Q>(value, unit);
            if (out_q.is_bad()) {
                is.setstate(std::ios_base::failbit);
            }
        } else if (!parse_quantity<Q>(out_q, value, unit)) {
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
    char quanity_string[128];
    char* unit;
    if (detail::extract_quantity(quanity_string, unit, sizeof(quanity_string), is)) {
        long double s = std::strtold(quanity_string, nullptr);
        value = Scalar(s);
        if (std::has_facet<facet>(is.getloc())) {
            o_q = std::use_facet<facet>(is.getloc()).format(value, unit);
            if (o_q.is_bad()) {
                is.setstate(std::ios_base::failbit);
            }
            return is;
        }
        o_q = detail::parse_standard_rep<Scalar, System>(unit);
        o_q.value *= value;
    }

    if (o_q.is_bad()) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

} // end of namespace dim
