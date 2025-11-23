#pragma once
#include "dim/dynamic_quantity.hpp"
#include "quantity_facet.hpp"
#include <string>
#ifdef __cpp_lib_format
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

#ifdef __cpp_lib_format
template <class Q, DIM_IS_QUANTITY(Q)> struct std::formatter<Q, char> {
    using facet = typename Q::system::facet;

    std::formatter<typename Q::scalar, char> scalar_format;

    template <class ParseContext> constexpr ParseContext::iterator parse(ParseContext& io_ctx)
    {
        return scalar_format(io_ctx);
    }

    template <class FmtContext> constexpr ParseContext::iterator format(Q i_quantity, FmtContext& io_ctx)
    {
        std::locale loc; // Get the global locale
        if (std::has_facet<facet>(loc)) {
            auto formatted = std::use_facet<facet>(loc).format(i_quantity);
            return std::format_to(scalar_format.format(formatted.value()), "_{}", formatted.symbol());
        }
        // FIXME default
        return ctx.out();
    }
}
#endif