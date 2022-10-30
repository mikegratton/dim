#pragma once
#include <map>
#include <cstring>
#include <cstdio>
#include <limits>
#include "base.hpp"
#include "system_creation_helper.hpp"
#include "dynamic_quantity.hpp"
#include "io_detail.hpp"

namespace dim
{
    
#define DIM_MAX_SYMBOL 64

/*
 * Basic formatting of a quantity's unit
 */
template<class Q, DIM_IS_QUANTITY(Q)>
char* print_unit(char* o_unit_str, Q const& q)
{
    bool spaceit = false;
    detail::print_unit<typename Q::unit>(o_unit_str, typename Q::unit(), spaceit);
    return o_unit_str;
}

/*
 * Basic formatting of a quantity
 */
template<class Q, DIM_IS_QUANTITY(Q)>
char* print_quantity(char* o_quant_str, Q const& q)
{
    int offset = sprintf(o_quant_str, "%g_", static_cast<double>(q.value));
    print_unit(o_quant_str+offset, q);
    return o_quant_str;
}

/*
 * Print a dynamic unit (specialized by each system)
 */
template<class System>
void print_unit(char* buf, dynamic_unit const& unit, bool spaceit)
{
    if (spaceit) {
        buf += sprintf(buf, "_");
    }
    sprintf(buf, "[%d %d %d %d %d %d %d %d]", 
        (int) unit[0], (int) unit[1], (int) unit[2], (int) unit[3], 
        (int) unit[4], (int) unit[5], (int) unit[6], (int) unit[7]);
}


/*
 * Formated output helper. The formatter class below produces these for output.
 */
template<class scalar=double, DIM_IS_SCALAR(scalar)>
class formatted_quantity
{
public:
    formatted_quantity(const char* symbol_ = nullptr, scalar value_ = static_cast<scalar>(1))
        : m_symbol(symbol_), m_value(value_) { }

    scalar value() const { return m_value; }

    const char* symbol() const { return m_symbol ? m_symbol : m_dyn_symbol; }

    char* set_symbol() { m_symbol = nullptr; return m_dyn_symbol; }

private:
    const char* m_symbol;
    scalar m_value;
    char m_dyn_symbol[DIM_MAX_SYMBOL];
};

/*
 * Instructions on how to format type Q. Links a symbol to an affine transform.
 * For input of a scalar s matching the symbol, Q <- s*scale + add
 * For output of q into s,symbol ("formatted_quantity"), s <- (q-add)/scale
 *
 * Example: formatter("feet", foot) creats a formatter that can take a double representing
 * "ft" and transform it into a Length in the same system as "foot", and it can take
 * a Length l and format it as feet for output regardless of the underlying rep.
 *
 */
template<class Q, DIM_IS_QUANTITY(Q)>
struct formatter : public detail::container_base {
    using scalar = typename Q::scalar;
    const char* symbol;
    
    formatter(const char* symbol_, Q const& scale_, Q const& add_ = Q(0))
        : symbol(symbol_), scale(scale_), add(add_) { }
        
    scalar non_dim(Q const& q) const { return (q - add) / scale; }
    
    formatted_quantity<scalar> output(Q const& q) const
    {
        return formatted_quantity<scalar>(symbol, non_dim(q));
    }
    
    Q input(scalar const& s) const { return s * scale + add; }
    
    static long index() { return ::dim::index<typename Q::unit>(); }
    
private:
    const Q scale;
    const Q add;
};

// Map from char to a formatter object (for input)
template<class Q, DIM_IS_QUANTITY(Q)>
using format_map = std::map<const char*, formatter<Q>, detail::char_less>;

/*
 * Create template specializations of this function for your system to control the
 * default formats available for input
 */
template<class Q, DIM_IS_QUANTITY(Q)>
format_map<Q> const& get_default_format()
{
    static const format_map<Q> EMPTY;
    return EMPTY;
}


template<class S, class System>
struct dynamic_formatter : public detail::container_base {
    using scalar = S;
    using dynamic = dynamic_quantity<S, System>;
    const char* symbol;
    dynamic_formatter(const char* symbol_,  dynamic const& scale_, dynamic const& add_ = dynamic::bad())
        : symbol(symbol_)
        , scale(scale_)
        , add(add_)
    {
        if(add.is_bad()) {
            add = dynamic(0, scale.unit);
        }
    }
    scalar non_dim(dynamic const& q) const { return ((q + add) / scale).value; }
    formatted_quantity<scalar> output(dynamic const& q) const
    {
        return formatted_quantity<scalar>(symbol, non_dim(q));
    }
    dynamic input(scalar const& s) const { return dynamic(scale.value*s, scale.unit) + add; }
    long index() { return ::dim::index(scale.unit); }
private:
    const dynamic scale;
    const dynamic add;
};

// Map from char to a formatter object (for input)
template<class S, class System>
using dynamic_format_map = std::map<const char*, dynamic_formatter<S, System>, detail::char_less>;


/*
 * Default parsing for quantities. Returns false if unit_str is wrong for Q.
 * Works in two phases:
 * (1) Look up unit_str in unit_map (backed by the default_format map if not specified)
 * (2) If not found, use Q::system's dynamic quantity parser
 */
template<class Q, DIM_IS_QUANTITY(Q)>
bool parse_quantity(Q& o_q, double value, const char* unit_str,
                    format_map<Q> const& unit_map = get_default_format<Q>())
{
    auto it = unit_map.find(unit_str);
    if(it != unit_map.end()) {
        o_q = it->second.input(value);
        return true;
    }
    using scalar = typename Q::scalar;
    scalar scale;
    if(detail::parse_unit_dynamic<typename Q::unit, scalar>(scale, unit_str)) {
        o_q = Q(value*scale);
        return true;
    }
    o_q = Q::bad_quantity();
    return false;
}
} // end of namespace dim
