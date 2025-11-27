#pragma once
#include <algorithm>
#include <cstring>
#include <vector>
#include <initializer_list>
#include "dim/io_detail.hpp"
#include "dynamic_quantity.hpp"
#include "io.hpp"

namespace dim
{

template <class Container, class Key, class Compare, class Equal>
typename Container::const_iterator find(Container const& container, Key const& key, Compare const& comp, Equal const& equal)
{
    auto it = std::lower_bound(container.begin(), container.end(), key, comp);
    if (it == container.end() || !equal(*it, key)) {
        return container.end();
    }
    return it;
}

template <class Container, class Key, class Compare, class Equal>
typename Container::iterator find(Container& container, Key const& key, Compare const& comp, Equal const& equal)
{
    auto it = std::lower_bound(container.begin(), container.end(), key, comp);
    if (it == container.end() || !equal(*it, key)) {
        return container.end();
    }
    return it;
}


/// Format a scalar plus string to quantity or dynamic_quantity.
///
/// Each of these is for a given quantity (denoted by its index aka dynamic_unit).
/// If a quantity doesn't match this index, this returns a bad quantity.
template <class Scalar, class System> class input_format_map
{
  public:
    using system = System;
    using scalar = Scalar;
    using formatter_type = formatter<Scalar, System>;
    using quantity_type = dynamic_quantity<Scalar, System>;
    using unit_type = typename quantity_type::unit_type;

  private:
    using const_iterator = typename std::vector<formatter_type>::const_iterator;
    using iterator = typename std::vector<formatter_type>::iterator;

  public:

    explicit input_format_map(unit_type unit_code_)
        : mindex(unit_code_)
    {
    }

    template<class U, DIM_IS_UNIT(U)>
    explicit input_format_map(U const& unit_code_)
        : mindex(unit_code_)
    {
    }

    explicit input_format_map(formatter_type first_formater)
        : mindex(first_formater.index())
    {
        insert(first_formater);
    }
    
    input_format_map(std::initializer_list<formatter_type> const& list)
    : mindex(list.size() > 0? formatter_type(*list.begin()).index() : unit_type(~0ul))
    {
        for (auto const& item : list) {            
            insert(item);
        }
    }

    template <class Q, DIM_IS_QUANTITY(Q)> static input_format_map make_map() { return input_format_map(::dim::index<Q>()); }

    template <class Q, DIM_IS_QUANTITY(Q)> bool insert(char const* s, Q const& scale, Q const& add = Q(0))
    {
        return insert(formatter_type(s, scale, add));
    }

    bool insert(formatter_type const& item)
    {
        if (item.index() != index()) {
            return false;
        }
        auto it = find(item.symbol());
        if (it != sorted_data.end()) {            
            *it = item;
            return true;
        }
        sorted_data.push_back(item);
        std::sort(sorted_data.begin(), sorted_data.end(), compare_formatter);
        return true;
    }

    template <class Q, DIM_IS_QUANTITY(Q)> Q to_quantity(typename Q::scalar const& s, char const* symbol) const
    {
        if (::dim::index<Q>() != index()) {
            return Q::bad_quantity();
        }
        auto it = find(symbol);
        return (it != sorted_data.end()? it->template input<Q>(s) : Q::bad_quantity());
    }

    quantity_type to_quantity(Scalar const& s, char const* symbol) const
    {
        auto it = find(symbol);
        return (it != sorted_data.end()? it->input(s) : quantity_type::bad_quantity());
    }

    formatter_type const* get(char const* symbol) const 
    {
        auto it = find(symbol);
        return (it != sorted_data.end()? &(*it) : nullptr);
    }

    std::size_t size() const { return sorted_data.size(); }

    void clear() { sorted_data.clear(); }

    unit_type index() const { return mindex; }

    bool erase(char const* symbol) 
    {
        auto it = find(symbol);
        if (it != sorted_data.end()) {
            sorted_data.erase(it);
            return true;
        }
        return false;
    }

  private:
    const_iterator find(char const* symbol) const
    {
        return ::dim::find(sorted_data, symbol, compare_item_formatter, equal_item_formatter);
    }

    iterator find(char const* symbol)
    {
        return ::dim::find(sorted_data, symbol, compare_item_formatter, equal_item_formatter);        
    }

    static bool compare_formatter(formatter_type const& left, formatter_type const& right)
    {
        return strncmp(left.symbol(), right.symbol(), kMaxSymbol) < 0;
    }

    static bool compare_item_formatter(formatter_type const& left, char const* symbol)
    {
        return strncmp(left.symbol(), symbol, kMaxSymbol) < 0;
    }

    static bool equal_item_formatter(formatter_type const& left, char const* symbol)
    {
        return strncmp(left.symbol(), symbol, kMaxSymbol) == 0;
    }

    unit_type mindex;
    std::vector<formatter_type> sorted_data;
};


/// A group of input_format_maps organized by index. 
///
/// For each index, there's an input_format_map of formatters representing
/// different symbols for that quantity.
template <class Scalar, class System> class input_format_map_group
{
  public:
    using map_type = input_format_map<Scalar, System>;
    using formatter_type = typename map_type::formatter_type;
    using quantity_type = typename map_type::quantity_type;
    using unit_type = typename quantity_type::unit_type;

  private:
    using const_iterator = typename std::vector<map_type>::const_iterator;
    using iterator = typename std::vector<map_type>::iterator;

  public:

    template <class Q, DIM_IS_QUANTITY(Q)> bool insert(char const* s, Q const& scale, Q const& add = Q(0))
    {
        return insert(formatter_type(s, scale, add));
    }

    bool insert(formatter_type const& item)
    {
        iterator it = find(item.index());
        if (it != sorted_data.end()) {
            return it->insert(item);
        }
        map_type new_map(item.index());
        new_map.insert(item);
        sorted_data.push_back(new_map);
        std::sort(sorted_data.begin(), sorted_data.end(), compare_formatter);
        return true;
    }

    bool insert(map_type const& whole_map)
    {
        auto it = find(whole_map.index());
        if (it != sorted_data.end()) {
            *it = whole_map;
            return true;
        }
        sorted_data.push_back(whole_map);
        std::sort(sorted_data.begin(), sorted_data.end(), compare_formatter);
        return true;
    }

    bool erase(unit_type index)
    {
        auto it = find(index);
        if (it != sorted_data.end()) {
            sorted_data.erase(it);
            return true;
        }
        return false;
    }

    bool erase(unit_type index, char const* symbol)
    {
        auto it = find(index);
        if (it != sorted_data.end()) {
            bool status = it->erase(symbol);
            if (it->size() == 0) {
                sorted_data.erase(it);
            }
            return status;
        }
        return false;
    }

    template <class Q, DIM_IS_QUANTITY(Q)> Q to_quantity(typename Q::scalar const& s, char const* symbol) const
    {
        auto it = find(::dim::index<Q>());
        return (it != sorted_data.end()? it->template to_quantity<Q>(s, symbol) : Q::bad_quantity());
    }

    quantity_type to_quantity(Scalar const& s, char const* symbol) const
    {
        for (auto const& map : sorted_data) {
            quantity_type value = map.to_quantity(s, symbol);
            if (!value.is_bad()) {
                return value;
            }
        }
        return quantity_type::bad_quantity();
    }

    map_type const* get(unit_type const& u) const 
    {
        auto it = find(u);
        return (it != sorted_data.end()? &(*it) : nullptr);
    }

    void clear() { sorted_data.clear(); }

    std::size_t size() const { return sorted_data.size(); }

  private:
    const_iterator find(unit_type id) const
    {
        return ::dim::find(sorted_data, id, compare_item_formatter, equal_item_formatter);
    }

    iterator find(unit_type id)
    {
        return ::dim::find(sorted_data, id, compare_item_formatter, equal_item_formatter);
    }

    static bool compare_formatter(map_type const& left, map_type const& right) { return left.index() < right.index(); }

    static bool compare_item_formatter(map_type const& left, unit_type right) { return left.index() < right; }

    static bool equal_item_formatter(map_type const& left, unit_type right) { return left.index() == right; }

    std::vector<map_type> sorted_data;
};

/// A map of formatters to turn quantities into (scalar, string) pairs.
///
/// Formatters are organized by a quantity's index. Each dynamic_unit 
/// may have one formatter.
template <class Scalar, class System> class output_format_map
{
  public:
    using system = System;
    using scalar = Scalar;
    using formatter_type = formatter<Scalar, System>;
    using formatted = typename formatter_type::formatted;
    using quantity_type = dynamic_quantity<Scalar, System>;
    using unit_type = typename quantity_type::unit_type;

  private:
    using const_iterator = typename std::vector<formatter_type>::const_iterator;
    using iterator = typename std::vector<formatter_type>::iterator;

  public:
    template <class Q, DIM_IS_QUANTITY(Q)> formatted format(Q const& q) const
    {
        auto it = find(::dim::index<Q>());
        return (it != sorted_data.end()? it->template output<Q>(q) : formatted_quantity<scalar>::bad_format());        
    }

    formatted format(quantity_type const& q) const
    {
        auto it = find(q.unit());
        return (it != sorted_data.end()? it->output(q) : formatted_quantity<scalar>::bad_format());        
    }

    template <class Q, DIM_IS_QUANTITY(Q)> bool insert(char const* s, Q const& scale, Q const& add = Q(0))
    {
        return insert(formatter_type(s, scale, add));
    }

    bool insert(formatter_type const& item)
    {
        auto it = find(item.index());
        if (it != sorted_data.end()) {
            *it = item;
            return true;
        }
        sorted_data.push_back(item);
        std::sort(sorted_data.begin(), sorted_data.end(), compare_formatter);
        return true;
    }

    formatter_type const* get(unit_type const& u) const 
    {
        auto it = find(u);
        return (it != sorted_data.end()? &(*it) : nullptr);
    }

    bool erase(unit_type index)
    {
        auto it = find(index);
        if (it != sorted_data.end()) {
            sorted_data.erase(it);
            return true;
        }
        return false;
    }

    void clear() { sorted_data.clear(); }

    std::size_t size() const { return sorted_data.size(); }

  private:
    const_iterator find(unit_type id) const
    {
        return ::dim::find(sorted_data, id, compare_item_formatter, equal_item_formatter);
    }

    iterator find(unit_type id)
    {
        return ::dim::find(sorted_data, id, compare_item_formatter, equal_item_formatter);        
    }

    static bool compare_formatter(formatter_type const& left, formatter_type const& right)
    {
        return left.index() < right.index();
    }

    static bool compare_item_formatter(formatter_type const& left, unit_type right) { return left.index() < right; }

    static bool equal_item_formatter(formatter_type const& left, unit_type right) { return left.index() == right; }

    std::vector<formatter_type> sorted_data;
};

template <class Q> using static_input_format_map = input_format_map<typename Q::scalar, typename Q::unit::system>;

/**
 * @brief Create template specializations of this function for your system to control the
 * default formats available for input
 */
template <class Q> input_format_map<typename Q::scalar, typename Q::system> const& get_default_format()
{
    input_format_map<typename Q::scalar, typename Q::system> const static EMPTY(::dim::index<Q>());
    return EMPTY;
}

/**
 * @brief Default input parsing for quantities.
 * Returns false if unit_str is wrong for Q.
 * Works in two phases:
 * (1) Look up unit_str in unit_map (backed by the default_format map if not specified)
 * (2) If not found, use Q::system's dynamic quantity parser
 */
template <class Q, DIM_IS_QUANTITY(Q)>
bool parse_quantity(Q& o_q, formatted_quantity<typename Q::scalar> const& formatted,
                    input_format_map<typename Q::scalar, typename Q::system> const& unit_map = get_default_format<Q>())
{
    o_q = unit_map.template to_quantity<Q>(formatted.value(), formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }
    auto dynamic_q = detail::parse_standard_rep<typename Q::scalar, typename Q::system>(formatted.symbol());
    o_q = (formatted.value() * dynamic_q).template as<Q>();
    return !(o_q.is_bad());    
}

template <class Scalar, class System>
bool parse_quantity(dynamic_quantity<Scalar, System>& o_q, formatted_quantity<Scalar> const& formatted,
                    input_format_map<Scalar, System> const& unit_map)
{
    o_q = unit_map.to_quantity(formatted.value(), formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }    
    o_q = formatted.value() * detail::parse_standard_rep<Scalar, System>(formatted.symbol());
    return !o_q.is_bad();
}

template <class Q, DIM_IS_QUANTITY(Q)>
bool format_quantity(formatted_quantity<typename Q::scalar>& o_formatted, Q const& i_q,
                     output_format_map<typename Q::scalar, typename Q::system> const* out_map = nullptr)
{
    if (out_map) {
        o_formatted = out_map->format(i_q);
        if (!o_formatted.is_bad()) {
            return true;
        }
    }
    o_formatted = formatted_quantity<typename Q::scalar>(nullptr, dimensionless_cast(i_q));
    print_unit(o_formatted.set_symbol(), o_formatted.set_symbol() + kMaxSymbol, i_q);
    return true;
}

template <class Scalar, class System>
bool format_quantity(formatted_quantity<Scalar>& o_formatted, dynamic_quantity<Scalar, System> const& i_q,
                     output_format_map<Scalar, System> const* out_map = nullptr)
{
    if (out_map) {
        o_formatted = out_map->format(i_q);
        if (!o_formatted.is_bad()) {
            return true;
        }
    }
    o_formatted = formatted_quantity<Scalar>(nullptr, dimensionless_cast(i_q));
    print_unit(o_formatted.set_symbol(), o_formatted.set_symbol() + kMaxSymbol, i_q);
    return !o_formatted.is_bad();
}

} // namespace dim

