#pragma once
#include <cstdio>
#include <cstring>
#include <limits>
#include "io_detail.hpp"
#include "dynamic_quantity.hpp"

namespace dim {

/**
 * @brief A quantity represented as a scalar number and a symbol (char* string).
 */
template <class scalar = double, DIM_IS_SCALAR(scalar)>
class formatted_quantity {
   public:
    formatted_quantity(const char* symbol_ = nullptr, scalar value_ = static_cast<scalar>(1))
        : m_symbol(symbol_), m_value(value_)
    {
    }

    static formatted_quantity bad_format() { return formatted_quantity(nullptr, std::numeric_limits<scalar>::quiet_NaN()); }

    bool is_bad() const { return isbad__(m_value); }

    scalar value() const { return m_value; }

    const char* symbol() const { return m_symbol ? m_symbol : m_dyn_symbol; }

    char* set_symbol()
    {
        m_symbol = nullptr;
        return m_dyn_symbol;
    }

   private:
    const char* m_symbol;
    scalar m_value;
    char m_dyn_symbol[kMaxSymbol];
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

    formatter(char const* symbol_, dynamic_type const& scale_, dynamic_type const& add_ = dynamic_type::bad())
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
        return ( result.dimensionless()? dimensionless_cast(result) : std::numeric_limits<scalar>::quiet_NaN() );
    }

    formatted output(dynamic_type const& q) const
    {
        return formatted(symbol(), non_dim(q));
    }

    template <class Q, DIM_IS_QUANTITY(Q)> formatted output(Q const& q) const
    {
        return formatted(symbol(), non_dim(q));
    }

    template<class Q, DIM_IS_QUANTITY(Q)>
    Q input(scalar const& s) const { return (s * scale + add).template as<Q>(); }

    dynamic_type input(scalar const& s) const { return scale * dynamic_type(s) + add; }

    dynamic_unit<System> index() const { return scale.unit(); }

    char const* symbol() const { return m_symbol; }

   private:
    char m_symbol[kMaxSymbol];
    dynamic_type scale;
    dynamic_type add;
};


/**
 * @brief Basic formatting of a quantity's unit. Lowest level version
 */
template <class Q, DIM_IS_QUANTITY(Q)>
char* print_unit(char* o_unit_str)
{
    bool spaceit = false;
    detail::print_unit(o_unit_str, dim::index<Q>(), spaceit);
    return o_unit_str;
}

/**
 * @brief Basic formatting of a dynamic_unit. Lowest level version
 */
template <class System, DIM_IS_SYSTEM(System)>
char* print_unit(char* buf, dynamic_unit<System> const& unit)
{
    bool spaceit = false;
    detail::print_unit(buf, unit);
    return buf;
}

/**
 * @brief Basic formatting of a quantity. Lowest level version
 */
template <class Q, DIM_IS_QUANTITY(Q)>
char* print_quantity(char* o_quant_str, Q const& q)
{
    int offset = sprintf(o_quant_str, "%g_", dimensionless_cast(q));    
    print_unit<Q>(o_quant_str + offset);
    return o_quant_str;
}

}  // end of namespace dim
