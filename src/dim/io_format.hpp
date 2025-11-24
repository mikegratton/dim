#pragma once
#include "dim/dynamic_quantity.hpp"
#include "quantity_facet.hpp"
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
    if (std::has_facet<facet>(loc)) {
        auto formatted = std::use_facet<facet>(loc).format(i_quantity);
        return std::to_string(formatted.value()) + '_' + formatted.symbol();
    }
    char raw[64];
    print_quantity(raw, i_quantity);
    return std::string(raw);
}

/// Make a string representation of Q using the facet
template <class Scalar, class System> std::string to_string(dynamic_quantity<Scalar, System> i_quantity)
{
    using facet = typename System::facet;
    std::locale loc; // Get the global locale
    if (std::has_facet<facet>(loc)) {
        auto formatted = std::use_facet<facet>(loc).format(i_quantity);
        return std::to_string(formatted.value()) + '_' + formatted.symbol();
    }
    char raw[64];
    print_quantity(raw, i_quantity);
    return std::string(raw);
}

/// Parse a string representation to Q using the facet. If the string
/// does not represent a quantity of type Q, return false
template <class Q, DIM_IS_QUANTITY(Q)> bool from_string(Q& o_quantity, std::string const& i_string)
{
    using facet = typename Q::system::facet;
    using scalar = typename Q::scalar;
    char* endOfNumber;
    scalar value = std::strtold(i_string.data(), &endOfNumber);
    char const* unit_string = endOfNumber;
    std::locale loc; // Get the global locale
    if (std::has_facet<facet>(loc)) {
        unit_string = detail::advance_past_separator(unit_string);
        o_quantity = std::use_facet<facet>(loc).template format<Q>(value, unit_string);
        return o_quantity.is_bad() ? false : true;
    }
    return parse_quantity<Q>(o_quantity, value, unit_string);
}

/// Parse a string representation to Q using the facet. If the string
/// does not represent a quantity of type Q, return false
template <class Scalar, class System>
bool from_string(dynamic_quantity<Scalar, System>& o_quantity, std::string const& i_string)
{
    using facet = typename System::facet;
    char* endOfNumber;
    Scalar value = std::strtold(i_string.data(), &endOfNumber);
    char const* unit_string = endOfNumber;
    std::locale loc; // Get the global locale
    if (std::has_facet<facet>(loc)) {
        o_quantity = std::use_facet<facet>(loc).format(value, unit_string);
        return o_quantity.is_bad() ? false : true;
    }
    return parse_quantity(o_quantity, value, unit_string);
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
        if (std::has_facet<facet>(io_ctx.locale())) {
            auto formatted = std::use_facet<facet>(io_ctx.locale()).format(i_quantity);
            std::formatter<scalar>::format(formatted.value(), io_ctx);
            return insert_symbol(io_ctx, formatted.symbol());
        } 
        std::formatter<scalar>::format(dimensionless_cast(i_quantity), io_ctx);
        char buffer[32];
        return insert_symbol(io_ctx, dim::print_unit<Q>(buffer));
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
        if (std::has_facet<facet>(io_ctx.locale())) {
            auto formatted = std::use_facet<facet>(io_ctx.locale()).format(i_quantity);
            std::formatter<double>::format(formatted.value(), io_ctx);
            return insert_symbol(io_ctx, formatted.symbol());
        }
        std::formatter<double>::format(dimensionless_cast(i_quantity), io_ctx);
        char buffer[32];
        return insert_symbol(io_ctx, dim::print_unit(buffer, i_quantity.unit()));
    }
};
#endif