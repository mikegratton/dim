#pragma once
#include <cstdio>
#include <cstring>
#include <limits>

#include "base.hpp"
#include "dynamic_quantity.hpp"
#include "io_detail.hpp"
#include "system_creation_helper.hpp"

namespace dim {

constexpr int kMaxSymbol = 32;

/**
 * @brief Basic formatting of a quantity's unit. Lowest level version
 */
template <class Q, DIM_IS_QUANTITY(Q)>
char* print_unit(char* o_unit_str, Q const& q)
{
    bool spaceit = false;
    detail::print_unit<typename Q::unit>(o_unit_str, typename Q::unit(), spaceit);
    return o_unit_str;
}

/**
 * @brief Basic formatting of a quantity. Lowest level version
 */
template <class Q, DIM_IS_QUANTITY(Q)>
char* print_quantity(char* o_quant_str, Q const& q)
{
    int offset = sprintf(o_quant_str, "%g_", dimensionless_cast(q));
    print_unit(o_quant_str + offset, q);
    return o_quant_str;
}

/**
 * @brief Print a dynamic unit (specialized by each system)
 */
template <class System>
void print_unit(char* buf, dynamic_unit const& unit, bool spaceit)
{
    if (spaceit) { buf += sprintf(buf, "_"); }
    sprintf(buf, "[%d %d %d %d %d %d %d %d]", (int)unit[0], (int)unit[1], (int)unit[2], (int)unit[3], (int)unit[4],
            (int)unit[5], (int)unit[6], (int)unit[7]);
}

/**
 * @brief Formated output helper. The formatter class below produces these for output.
 */
template <class scalar = double, DIM_IS_SCALAR(scalar)>
class formatted_quantity {
   public:
    formatted_quantity(const char* symbol_ = nullptr, scalar value_ = static_cast<scalar>(1))
        : m_symbol(symbol_), m_value(value_)
    {
    }

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
 *@brief Instructions on how to format type Q.
 *
 * Links a symbol to an affine transform.
 * For input of a scalar s matching the symbol, Q <- s*scale + add
 * For output of q into s,symbol ("formatted_quantity"), s <- (q-add)/scale
 *
 * Example: formatter("feet", foot) creats a formatter that can take a double representing
 * "ft" and transform it into a Length in the same system as "foot", and it can take
 * a Length l and format it as feet for output regardless of the underlying rep.
 *
 */
template <class Q, DIM_IS_QUANTITY(Q)>
struct formatter : public detail::container_base {
    using quantity = Q;
    using scalar = typename quantity::scalar;

    formatter() : scale(Q(1.0)), add(Q(0.0)) { sym[0] = 0; }

    formatter(const char* symbol_, Q const& scale_, Q const& add_ = Q(0)) : scale(scale_), add(add_)
    {
        strncpy(sym, symbol_, kMaxSymbol);
    }

    /// Obtain the scalar value of q nondimensionalized by the transform in this formatter
    scalar non_dim(Q const& q) const { return (q - add) / scale; }

    /// Perform output formatting
    formatted_quantity<scalar> output(Q const& q) const { return formatted_quantity<scalar>(symbol(), non_dim(q)); }

    /// Perform input formatting (affine transform)
    Q input(scalar const& s) const { return s * scale + add; }

    static constexpr uint64_t index() { return Q::index(); }

    char const* symbol() const { return sym; }

   private:
    char sym[kMaxSymbol];
    const Q scale;
    const Q add;
};

/// Format dynamic_quantities
template <class S, class System>
struct dynamic_formatter : public detail::container_base {
    using scalar = S;
    using dynamic = dynamic_quantity<S, System>;

    dynamic_formatter(const char* symbol_, dynamic const& scale_, dynamic const& add_ = dynamic::bad())
        : scale(scale_), add(add_)
    {
        strncpy(sym, symbol_, kMaxSymbol);
        if (add.is_bad()) { add = dynamic(0, scale.unit); }
    }
    dynamic_formatter() = default;

    scalar non_dim(dynamic const& q) const { return dimensionless_cast((q + add) / scale); }
    formatted_quantity<scalar> output(dynamic const& q) const { return formatted_quantity<scalar>(symbol, non_dim(q)); }
    dynamic input(scalar const& s) const { return dynamic(scale.value * s, scale.unit) + add; }
    uint64_t index() { return ::dim::index(scale.unit); }
    char const* symbol() const { return sym; }

   private:
    char sym[kMaxSymbol];
    const dynamic scale;
    const dynamic add;
};

}  // end of namespace dim
