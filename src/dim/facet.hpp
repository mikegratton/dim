#pragma once
#include "dim/io.hpp"
#include "dim/io_detail.hpp"
#include "format_map.hpp"
#include <locale>

namespace dim
{

/// An iostream facet for formatting quantities. This is typically used by operator<< and operator>>,
/// but can be adjusted to change how quantities are formated in a program
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
    input_format_map_group<scalar, system> input_symbol;
    output_format_map<scalar, system> output_symbol;

  public:
    explicit quantity_facet(std::size_t refs = 0)
        : std::locale::facet(refs)
    {
    }

    quantity_facet(quantity_facet const* other, std::size_t refs) = delete;

    /**
     * @brief Format a quantity for output (convert to correct scalar value, assign symbol)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    formatted format(Q const& q) const
    {
        formatted result;
        format_quantity(result, q, &output_symbol);
        return result;
    }

    /**
     * @brief Format a dynamic_quantity for output (convert to correct scalar value, assign symbol)
     */
    formatted format(dynamic_type const& q) const
    {
        formatted result;
        format_quantity(result, q, &output_symbol);
        return result;
    }

    /**
     * @brief Format a scalar + symbol into a quantity. If the conversion is illegal, the value
     * will be NaN (check with is_bad() method on quantity)
     *
     * @note This form is not preferred as it can suffer from buffer overruns if symbol is not null terminated
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q format(typename Q::scalar const& s, char const* symbol) const
    {
        Q result = input_symbol.template to_quantity<Q>(s, symbol);
        if (!result.is_bad()) {
            return result;
        }
        auto dynamic_q =
            detail::parse_standard_rep<typename Q::scalar, typename Q::system>(symbol, symbol + kMaxSymbol);
        return (s * dynamic_q).template as<Q>();
    }

    /**
     * @brief Format a scalar + symbol into a quantity. If the conversion is illegal, the value
     * will be NaN (check with is_bad() method on quantity)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q format(formatted const& f) const
    {
        Q q;
        input_format_map<Scalar, System> const* input_format = input_symbol.get(index(q));
        if (input_format) {
            parse_quantity(q, f, *input_format);
        } else {
            parse_quantity(q, f);
        }
        return q;
    }

    /**
     * @brief Format a scalar and a symbol to a dynamic_quantity
     * @note This form is not preferred as it can suffer from buffer overruns if symbol is not null terminated
     */
    dynamic_type format(Scalar const& s, char const* symbol) const
    {
        dynamic_type map_result = input_symbol.to_quantity(s, symbol);
        if (!map_result.is_bad()) {
            return map_result;
        }
        return detail::parse_standard_rep<Scalar, System>(symbol, symbol + kMaxSymbol) * dynamic_type(s);
    }

    /**
     * @brief Format a scalar and a symbol to a dynamic_quantity
     */
    dynamic_type format(formatted const& f) const
    {
        dynamic_type q;
        parse_quantity(q, f, input_symbol);
        return q;
    }

    /**
     * @brief Attach a new output formatter for a quantity, replacing the existing formatter
     *
     * @param f The new output formatter for quantity
     */
    void output_formatter(formatter_type const& f) { output_symbol.insert(f); }

    /**
     * @brief Attach a new output formatter for Q, replacing the existing formatter
     *
     * @param symbol Symbol for format
     * @param scale  One unit of symbol is this much of Q
     * @param add    (For affine maps, defaults to zero)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void output_formatter(char const* symbol, Q scale, Q add = Q(0.0))
    {
        output_symbol.insert(symbol, scale, add);
    }

    /**
     * @brief Attach a new input formatter for a unit class, adding it to the map
     * for this type, replacing any previous formatter with the same symbol.
     */
    void input_formatter(formatter_type const& f) { input_symbol.insert(f); }

    /**
     * @brief Replace the format map for a whole unit class
     */
    void input_formatter(input_format_map<Scalar, System> const& map) { input_symbol.insert(map); }

    /**
     * @brief Drop input formatters for Q (reverting to default format)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void clear_input_formatter()
    {
        input_symbol.erase(Q::index());
    }

    /**
     * @brief Drop all output formatters for Q (reverting to default format)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void clear_output_formatter()
    {
        output_symbol.erase(Q::index());
    }

    /**
     * @brief Drop ALL input formatters
     */
    void clear_input_formatters() { input_symbol.clear(); }

    /**
     * @brief Drop ALL output formatters
     */
    void clear_output_formatters() { output_symbol.clear(); }

    /**
     * @brief Drop ALL formatters for input and output
     */
    void clear()
    {
        clear_input_formatters();
        clear_output_formatters();
    }
};

} // namespace dim