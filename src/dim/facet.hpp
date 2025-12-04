#pragma once
#include "io.hpp"
#include "format_map.hpp"
#include <locale>

namespace dim
{

/**
 *  An iostream facet for formatting quantities. This is typically used by
 * operator<< and operator>>, but can be adjusted to change how quantities are
 * formated in a program.
 *
 * @note This is not a functional facet, but rather a base class for such facets.
 * To make a functional facet:
 * ```
 * class MyFacet : public quantity_facet<MyScalar, MySystem> {
 *   public:
 *    static std::locale::id id;
 * };
 * ```
 * That is, you need to add the mandatory id member (and define it in a cpp file).
 */
template <class Scalar, class System>
class quantity_facet : public std::locale::facet
{
  public:
    using scalar = Scalar;
    using system = System;
    using dynamic_type = dynamic_quantity<scalar, system>;
    using formatted = formatted_quantity<scalar>;
    using formatter_type = formatter<scalar, system>;

  private:
    input_format_map_group<scalar, system> m_input_symbol;
    output_format_map<scalar, system> m_output_symbol;

  public:
    explicit quantity_facet(std::size_t i_refs = 0)
        : std::locale::facet(i_refs)
    {
    }

    /**
     * @brief Format a quantity for output (convert to correct scalar value, assign symbol)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    formatted format(Q const& q) const
    {
        formatted result;
        format_quantity(result, q, &m_output_symbol);
        return result;
    }

    /**
     * @brief Format a dynamic_quantity for output (convert to correct scalar value, assign symbol)
     */
    formatted format(dynamic_type const& q) const
    {
        formatted result;
        format_quantity(result, q, &m_output_symbol);
        return result;
    }

    /**
     * @brief Format a scalar & symbol into a quantity. If the conversion is illegal, the value
     * will be NaN (check with is_bad() method on quantity)
     *
     * @note This form is not preferred as it can suffer from buffer overruns if symbol is not null terminated
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q format(typename Q::scalar const& i_scalar, char const* i_symbol) const
    {
        Q result = m_input_symbol.template to_quantity<Q>(i_scalar, i_symbol);
        if (!result.is_bad()) {
            return result;
        }
        auto dynamic_q =
            detail::parse_standard_rep<typename Q::scalar, typename Q::system>(i_symbol, i_symbol + kMaxSymbol);
        return (i_scalar * dynamic_q).template as<Q>();
    }

    /**
     * @brief Format a scalar & symbol into a quantity. If the conversion is illegal, the value
     * will be NaN (check with is_bad() method on quantity)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q format(formatted const& i_input) const
    {
        Q result;
        input_format_map<Scalar, System> const* input_format = m_input_symbol.get(index(result));
        if (input_format) {
            parse_quantity(result, i_input, *input_format);
        } else {
            parse_quantity(result, i_input);
        }
        return result;
    }

    /**
     * @brief Format a scalar & symbol to a dynamic_quantity.
     * @note This form is not preferred as it can suffer from buffer overruns if symbol is not null terminated
     */
    dynamic_type format(Scalar const& i_scalar, char const* i_symbol) const
    {
        dynamic_type map_result = m_input_symbol.to_quantity(i_scalar, i_symbol);
        if (!map_result.is_bad()) {
            return map_result;
        }
        return detail::parse_standard_rep<Scalar, System>(i_symbol, i_symbol + kMaxSymbol) * dynamic_type(i_scalar);
    }

    /**
     * @brief Format a scalar & symbol to a dynamic_quantity.
     */
    dynamic_type format(formatted const& i_input) const
    {
        dynamic_type result;
        parse_quantity(result, i_input, m_input_symbol);
        return result;
    }

    /**
     * @brief Attach a new output formatter for a quantity, replacing the existing formatter.
     *
     * @param i_format The new output formatter for quantity
     */
    void output_formatter(formatter_type const& i_format) { m_output_symbol.insert(i_format); }

    /**
     * @brief Attach a new output formatter for Q, replacing the existing formatter.
     *
     * @param i_symbol Symbol for format
     * @param i_scale  One unit of symbol is this much of Q
     * @param i_add    (For affine maps, defaults to zero)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void output_formatter(char const* i_symbol, Q i_scale, Q i_add = Q(0.0))
    {
        m_output_symbol.insert(i_symbol, i_scale, i_add);
    }

    /**
     * @brief Attach a new input formatter for a unit class, adding it to the map
     * for this type, replacing any previous formatter with the same symbol.
     */
    void input_formatter(formatter_type const& i_format) { m_input_symbol.insert(i_format); }

    /**
     * @brief Replace the format map for a whole unit class.
     */
    void input_formatter(input_format_map<Scalar, System> const& i_map) { m_input_symbol.insert(i_map); }

    /**
     * @brief Drop input formatters for Q (reverting to default format).
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void clear_input_formatter()
    {
        m_input_symbol.erase(Q::index());
    }

    /**
     * @brief Drop all output formatters for Q (reverting to default format).
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void clear_output_formatter()
    {
        m_output_symbol.erase(Q::index());
    }

    /**
     * @brief Drop ALL input formatters.
     */
    void clear_input_formatters() { m_input_symbol.clear(); }

    /**
     * @brief Drop ALL output formatters.
     */
    void clear_output_formatters() { m_output_symbol.clear(); }

    /**
     * @brief Drop ALL formatters for input and output.
     */
    void clear()
    {
        clear_input_formatters();
        clear_output_formatters();
    }
};

} // namespace dim