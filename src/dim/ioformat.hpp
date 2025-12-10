#pragma once
#include "DimConfig.hpp"
#include "format_map.hpp"
#include "io.hpp"
#include <locale>

#ifdef DIM_EXCEPTIONS
#include "dim/incommensurable_exception.hpp"
#endif

#ifdef DIM_STRING
#include <string>

namespace dim
{

/**
 * 1. Functions for serializing/deserializing quantities to/from strings.
 * 2. Functions for working with std::format
 */

/**
 * Make a string representation of Q using the facet.
 * @param i_quantity Quantity to serialize
 * @return Serialized quantity
 */
template <class Q, DIM_IS_QUANTITY(Q)>
std::string to_string(Q const& i_quantity)
{
    using facet = typename Q::system::facet;
    std::locale loc; // Get the global locale
    dim::formatted_quantity<typename Q::scalar> formatted;
    if (std::has_facet<facet>(loc)) {
        formatted = std::use_facet<facet>(loc).format(i_quantity);
    } else {
        format_quantity(formatted, i_quantity);
    }
    return std::to_string(formatted.value()) + '_' + formatted.symbol();
}

/**
 * Make a string representation of a dynamic_quantity using the facet.
 * @param i_quantity Quantity to serialize
 * @return Serialized quantity
 */
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
std::string to_string(DQ const& i_quantity)
{
    using facet = typename DQ::system::facet;
    std::locale loc; // Get the global locale
    dim::formatted_quantity<typename DQ::scalar> formatted;
    if (std::has_facet<facet>(loc)) {
        formatted = std::use_facet<facet>(loc).format(i_quantity);
    } else {
        format_quantity(formatted, i_quantity);
    }
    return std::to_string(formatted.value()) + '_' + formatted.symbol();
}

/**
 * Parse a string representation using the facet. If the string does not
 * represent a quantity of type Q, set o_quantity to a bad_quantity() and return
 * false.
 *
 * @param[out] o_quantity On exit, the deserialized quantity
 * @param[in] i_string String containing the serialized scalar and unit symbol
 * string.
 * @return True if parsing was successful.
 */
template <class Q, DIM_IS_QUANTITY(Q)>
bool from_string(Q& o_quantity, std::string const& i_string)
{
    using facet = typename Q::system::facet;
    using scalar = typename Q::scalar;
    formatted_quantity<scalar> formatted;
    auto result = from_chars(i_string.data(), i_string.data() + i_string.size(), formatted);
    if (result.ec != std::errc{}) {
        o_quantity = Q::bad_quantity();
        return false;
    }
    std::locale loc; // Get the global locale
#ifdef DIM_EXCEPTIONS
    try {
        if (std::has_facet<facet>(loc)) {
            o_quantity = std::use_facet<facet>(loc).template format<Q>(formatted);
            return true;
        }
        return parse_quantity<Q>(o_quantity, formatted);
    } catch (incommensurable_exception const& e) {
        o_quantity = Q::bad_quantity();
        return false;
    }
#else
    if (std::has_facet<facet>(loc)) {
        o_quantity = std::use_facet<facet>(loc).template format<Q>(formatted);
        return o_quantity.is_bad() ? false : true;
    }
    return parse_quantity<Q>(o_quantity, formatted);
#endif
}

/**
 * Parse a string representation using the facet. If the string does not
 * represent a dynamic_quantity of any known type, set o_quantity to a
 * bad_quantity() and return false.
 *
 * @param[out] o_quantity On exit, the deserialized dynamic_quantity
 * @param[in] i_string String containing the serialized scalar and unit symbol
 * string.
 * @return True if parsing was successful.
 */
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
bool from_string(DQ& o_quantity, std::string const& i_string)
{
    using facet = typename DQ::system::facet;
    using scalar = typename DQ::scalar;
    formatted_quantity<scalar> formatted;
    auto result = from_chars(i_string.data(), i_string.data() + i_string.size(), formatted);
    if (result.ec != std::errc{}) {
        o_quantity = DQ::bad_quantity();
        return false;
    }
    std::locale loc; // Get the global locale
#ifdef DIM_EXCEPTIONS
    try {
        if (std::has_facet<facet>(loc)) {
            o_quantity = std::use_facet<facet>(loc).format(formatted);
            return true;
        }
        return parse_quantity(o_quantity, formatted);
    } catch (incommensurable_exception const& e) {
        o_quantity = DQ::bad_quantity();
        return false;
    }
#else
    if (std::has_facet<facet>(loc)) {
        o_quantity = std::use_facet<facet>(loc).format(formatted);
        return o_quantity.is_bad() ? false : true;
    }
    return parse_quantity(o_quantity, formatted);
#endif
}

} // namespace dim
#endif

#if __cplusplus >= 202002L
#include <format>
/**
 * Provide a std::formattter for formatted_quantity
 *
 * This works like `std::format("{:6.3}", my_formatted)`, allowing the usual
 * format codes for the scalar type.  
 */
template <class Scalar>
    requires std::floating_point<Scalar>
struct std::formatter<dim::formatted_quantity<Scalar>> : std::formatter<Scalar> {
    using scalar = Scalar;    

    constexpr auto parse(std::format_parse_context& io_ctx) { return std::formatter<scalar>::parse(io_ctx); }

    auto format(dim::formatted_quantity<scalar> const& i_formatted, std::format_context& io_ctx) const
    {
        auto it = std::formatter<scalar>::format(i_formatted.value(), io_ctx);
        *it++ = '_';
        return std::formatter<char const*>{}.format(i_formatted.symbol(), io_ctx);
    }
};

/**
 * Provide a std::formattter for quantities.
 *
 * This works like `std::format("{:6.3}", my_quantity)`, allowing the usual
 * format codes for the scalar type.  The unit is formatted using the locale if
 * available.
 */
template <class Q>
    requires std::derived_from<Q, dim::quantity_tag>
struct std::formatter<Q> : std::formatter<dim::formatted_quantity<typename Q::scalar>> {
    using scalar = typename Q::scalar;
    using facet = typename Q::system::facet;

    constexpr auto parse(std::format_parse_context& io_ctx) { return std::formatter<dim::formatted_quantity<scalar>>::parse(io_ctx); }

    auto format(Q const& i_quantity, std::format_context& io_ctx) const
    {
        dim::formatted_quantity<typename Q::scalar> formatted;
        if (std::has_facet<facet>(io_ctx.locale())) {
            formatted = std::use_facet<facet>(io_ctx.locale()).format(i_quantity);
        } else {
            format_quantity(formatted, i_quantity);
        }
        return std::formatter<dim::formatted_quantity<scalar>>::format(formatted, io_ctx);        
    }
};

/**
 * Provide a std::formattter for dynamic quantities.
 *
 * This works like `std::format("{:6.3}", my_quantity)`, allowing the usual
 * format codes for the scalar type.  The unit is formatted using the locale if
 * available.
 */
template <class DQ>
    requires std::derived_from<DQ, dim::dynamic_quantity_tag>
struct std::formatter<DQ> :  std::formatter<dim::formatted_quantity<typename DQ::scalar>> {
    using scalar = typename DQ::scalar;
    using facet = typename DQ::system::facet;

    constexpr auto parse(std::format_parse_context& io_ctx) { return std::formatter<dim::formatted_quantity<scalar>>::parse(io_ctx); }

    auto format(DQ const& i_quantity, std::format_context& io_ctx) const
    {
        dim::formatted_quantity<scalar> formatted;
        if (std::has_facet<facet>(io_ctx.locale())) {
            formatted = std::use_facet<facet>(io_ctx.locale()).format(i_quantity);
        } else {
            format_quantity(formatted, i_quantity);
        }
        return std::formatter<dim::formatted_quantity<scalar>>::format(formatted, io_ctx);
    }
};



#endif