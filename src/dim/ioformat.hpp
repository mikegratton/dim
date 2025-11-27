#pragma once
#include "io.hpp"
#include "format_map.hpp"
#include <string>
#if __cplusplus >= 202002L
#include <format>
#endif


namespace dim
{

/// Make a string representation of Q using the facet
template <class Q, DIM_IS_QUANTITY(Q)> std::string to_string(Q i_quantity)
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

/// Make a string representation of Q using the facet
template <class Scalar, class System> std::string to_string(dynamic_quantity<Scalar, System> i_quantity)
{
    using facet = typename System::facet;
    std::locale loc; // Get the global locale
    dim::formatted_quantity<Scalar> formatted;
    if (std::has_facet<facet>(loc)) {
        formatted = std::use_facet<facet>(loc).format(i_quantity);
    } else {
        format_quantity(formatted, i_quantity);
    }
    return std::to_string(formatted.value()) + '_' + formatted.symbol();
}

/// Parse a string representation to Q using the facet. If the string
/// does not represent a quantity of type Q, return false
template <class Q, DIM_IS_QUANTITY(Q)> bool from_string(Q& o_quantity, std::string const& i_string)
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
    if (std::has_facet<facet>(loc)) {
        o_quantity = std::use_facet<facet>(loc).template format<Q>(formatted);
        return o_quantity.is_bad() ? false : true;
    }
    return parse_quantity<Q>(o_quantity, formatted);
}

/// Parse a string representation to Q using the facet. If the string
/// does not represent a quantity of type Q, return false
template <class Scalar, class System>
bool from_string(dynamic_quantity<Scalar, System>& o_quantity, std::string const& i_string)
{
    using facet = typename System::facet;
    using scalar = Scalar;
    formatted_quantity<scalar> formatted;
    auto result = from_chars(i_string.data(), i_string.data() + i_string.size(), formatted);
    if (result.ec != std::errc{}) {
        o_quantity = dynamic_quantity<scalar, System>::bad_quantity();
        return false;
    }    
    std::locale loc; // Get the global locale
    if (std::has_facet<facet>(loc)) {
        o_quantity = std::use_facet<facet>(loc).template format(formatted);
        return o_quantity.is_bad() ? false : true;
    }
    return parse_quantity(o_quantity, formatted);
}

} // namespace dim

#if __cplusplus >= 202002L
template <class Q> 
    requires std::derived_from<Q, dim::quantity_tag>
struct std::formatter<Q> : std::formatter<typename Q::scalar> 
{    
    using scalar = typename Q::scalar;
    using facet = typename Q::system::facet;

    constexpr auto parse(std::format_parse_context& ctx)
    {
        return std::formatter<scalar>::parse(ctx);
    }

    auto insert_symbol(std::format_context& io_ctx, char const* symbol) const
    {
        auto it = io_ctx.out();
        *it++ = '_';
        while(*symbol) { *it++ = *symbol++; }
        return it;
    }

    auto format(Q const& i_quantity, std::format_context& io_ctx) const
    {
        dim::formatted_quantity<typename Q::scalar> formatted;
        if (std::has_facet<facet>(io_ctx.locale())) {
            formatted = std::use_facet<facet>(io_ctx.locale()).format(i_quantity);            
        } else {
            format_quantity(formatted, i_quantity);
        }
        std::formatter<scalar>::format(formatted.value(), io_ctx);
        return insert_symbol(io_ctx, formatted.symbol());
    }
};


template <class DQ> 
  requires  std::derived_from<DQ, dim::dynamic_quantity_tag>
struct std::formatter<DQ> : std::formatter<double>
{   
    using scalar = typename DQ::scalar;
    using facet = typename DQ::system::facet;

    constexpr auto parse(std::format_parse_context& ctx) 
    {
        return std::formatter<double>::parse(ctx);
    }

    auto insert_symbol(std::format_context& io_ctx, char const* symbol) const
    {
        auto it = io_ctx.out();
        *it++ = '_';
        while(*symbol) { *it++ = *symbol++; }
        return it;
    }

    auto format(DQ const& i_quantity, std::format_context& io_ctx) const
    {
        dim::formatted_quantity<scalar> formatted;
        if (std::has_facet<facet>(io_ctx.locale())) {
            formatted = std::use_facet<facet>(io_ctx.locale()).format(i_quantity);            
        } else {
            format_quantity(formatted, i_quantity);
        }
        std::formatter<double>::format(formatted.value(), io_ctx);
        return insert_symbol(io_ctx, formatted.symbol());
    }
};
#endif