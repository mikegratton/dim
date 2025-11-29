#pragma once
#include "dim/io_detail.hpp"
#include "dynamic_quantity.hpp"
#include "io.hpp"
#include <algorithm>
#include <cstring>
#include <initializer_list>
#include <vector>

namespace dim
{

namespace detail
{
/// Bisection search for an item. This uses an equality comparator to determine
/// if it has found the result.
template <class Container, class Key, class Compare, class Equal>
typename Container::const_iterator find(Container const& container, Key const& key, Compare const& comp,
                                        Equal const& equal)
{
    auto it = std::lower_bound(container.begin(), container.end(), key, comp);
    if (it == container.end() || !equal(*it, key)) {
        return container.end();
    }
    return it;
}

/// Bisection search for an item. This uses an equality comparator to determine
/// if it has found the result.
template <class Container, class Key, class Compare, class Equal>
typename Container::iterator find(Container& container, Key const& key, Compare const& comp, Equal const& equal)
{
    auto it = std::lower_bound(container.begin(), container.end(), key, comp);
    if (it == container.end() || !equal(*it, key)) {
        return container.end();
    }
    return it;
}
} // namespace detail

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
    /**
     * Create an empty map for a given unit_type.
     */
    explicit input_format_map(unit_type unit_code_)
        : mindex(unit_code_)
    {
    }

    /**
     * Create an empty map for the unit type given by U.
     */
    template <class U, DIM_IS_UNIT(U)>
    explicit input_format_map(U const& unit_code_)
        : mindex(unit_code_)
    {
    }

    /**
     * Create a map with the unit type given by first_formatter.index(). Insert
     * this formatter into the map.
     */
    explicit input_format_map(formatter_type first_formater)
        : mindex(first_formater.index())
    {
        insert(first_formater);
    }

    /**
     * Create a map with the given contents. The type will be set to that of the
     * first item. If there are no items, the type will be an invalid type.
     * Items with types not matching the first item will be rejected.
     */
    input_format_map(std::initializer_list<formatter_type> const& list)
        : mindex(list.size() > 0 ? formatter_type(*list.begin()).index() : unit_type::bad_unit())
    {
        for (auto const& item : list) {
            insert(item);
        }
    }

    /**
     * Add a formatter to the map. Q::unit must match the type of the map.
     * @return True if insertion occurred. False if the item has the wrong index
     */
    template <class Q, DIM_IS_QUANTITY(Q)> bool insert(char const* s, Q const& scale, Q const& add = Q(0))
    {
        return insert(formatter_type(s, scale, add));
    }

    /**
     * Add a formatter to the map. item.index() must match the type of the map.
     * @return True if insertion occurred. False if the item has the wrong index
     */
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

    /**
     * Transform the scalar/symbol pair into a quantity. If Q::unit is the wrong type, or symbol is not in the map,
     * return a bad_quantity().
     */
    template <class Q, DIM_IS_QUANTITY(Q)> Q to_quantity(typename Q::scalar const& s, char const* symbol) const
    {
        if (::dim::index<Q>() != index()) {
            return Q::bad_quantity();
        }
        auto it = find(symbol);
        return (it != sorted_data.end() ? it->template input<Q>(s) : Q::bad_quantity());
    }

    /**
     * Transform the scalar/symbol pair into a quantity. If Q::unit is the wrong type, or symbol is not in the map,
     * return a bad_quantity().
     */
    template <class Q, DIM_IS_QUANTITY(Q)> Q to_quantity(formatted_quantity<scalar> const& input) const
    {
        return to_quantity<Q>(input.value(), input.symbol());
    }

    /**
     * Transform the scalar/symbol pair into a dynamic_quantity. If the symbol is
     * not in the map, return a bad_quantity().
     */
    quantity_type to_quantity(Scalar const& s, char const* symbol) const
    {
        auto it = find(symbol);
        return (it != sorted_data.end() ? it->input(s) : quantity_type::bad_quantity());
    }

    /**
     * Transform the scalar/symbol pair into a quantity. If Q::unit is the wrong type, or symbol is not in the map,
     * return a bad_quantity().
     */
    quantity_type to_quantity(formatted_quantity<scalar> const& input) const
    {
        return to_quantity(input.value(), input.symbol());
    }

    /**
     * Get a pointer to a formatter by symbol type. If the symbol is unknown,
     * this returns a nullptr. Do not delete this pointer.
     */
    formatter_type const* get(char const* symbol) const
    {
        auto it = find(symbol);
        return (it != sorted_data.end() ? &(*it) : nullptr);
    }

    /**
     * Get the number of items in the map.
     */
    std::size_t size() const { return sorted_data.size(); }

    /**
     * Clear all items from the map.
     */
    void clear() { sorted_data.clear(); }

    /**
     * Obtain the unit type that this formatter handles.
     */
    unit_type index() const { return mindex; }

    /**
     * Remove a symbol from the map. Returns true if removal occured.
     */
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
    /**
     * Look up an entry by symbol
     */
    const_iterator find(char const* symbol) const
    {
        return detail::find(sorted_data, symbol, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Look up an entry by symbol
     */
    iterator find(char const* symbol)
    {
        return detail::find(sorted_data, symbol, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Compare formatters by symbol. For sorting the map.
     */
    static bool compare_formatter(formatter_type const& left, formatter_type const& right)
    {
        return strncmp(left.symbol(), right.symbol(), kMaxSymbol) < 0;
    }

    /**
     * Compare a formatter to a symbol. For bisection search.
     */
    static bool compare_item_formatter(formatter_type const& left, char const* symbol)
    {
        return strncmp(left.symbol(), symbol, kMaxSymbol) < 0;
    }

    /**
     * Compare a formatter to a symbol. For bisection search.
     */
    static bool equal_item_formatter(formatter_type const& left, char const* symbol)
    {
        return strncmp(left.symbol(), symbol, kMaxSymbol) == 0;
    }

    /// Unit type for this map
    unit_type mindex;

    /// Formatters sorted by symbol
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
    using formatted = typename formatter_type::formatted;
    using quantity_type = typename map_type::quantity_type;
    using unit_type = typename quantity_type::unit_type;

  private:
    using const_iterator = typename std::vector<map_type>::const_iterator;
    using iterator = typename std::vector<map_type>::iterator;

  public:
    /**
     * Insert a formatter constructed from these arguments into the map group.
     * If a map for index<Q>() exists, this formatter is added to that group.
     * Otherwise a new map is created and the formatter is added to the new map.
     */
    template <class Q, DIM_IS_QUANTITY(Q)> bool insert(char const* symbol, Q const& scale, Q const& add = Q(0))
    {
        return insert(formatter_type(symbol, scale, add));
    }

    /**
     * Insert a formatter into the map group. If a map for index<Q>() exists,
     * this formatter is added to that group. Otherwise a new map is created and
     * the formatter is added to the new map.
     */
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

    /**
     * Insert a map into the map group. Any existing map for the index() type is
     * destroyed.
     */
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

    /**
     * Erase a map by index type. Returns true if a map was removed
     */
    bool erase(unit_type index)
    {
        auto it = find(index);
        if (it != sorted_data.end()) {
            sorted_data.erase(it);
            return true;
        }
        return false;
    }

    /**
     * Erase a map entry by index type and symbol. If a formatter matching the index
     * and symbol is found, it is removed and the function returns true.
     */
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

    /**
     * Format a scalar/symbol pair into a quantity of type Q. If no map for
     * index<Q>() exists, or there is no matching symbol in the map, this returns
     * a bad_quantity.
     */
    template <class Q, DIM_IS_QUANTITY(Q)> Q to_quantity(typename Q::scalar const& s, char const* symbol) const
    {
        auto it = find(::dim::index<Q>());
        return (it != sorted_data.end() ? it->template to_quantity<Q>(s, symbol) : Q::bad_quantity());
    }

    /**
     * Format a scalar/symbol pair into a quantity of type Q. If no map for
     * index<Q>() exists, or there is no matching symbol in the map, this returns
     * a bad_quantity.
     */
    template <class Q, DIM_IS_QUANTITY(Q)> Q to_quantity(formatted const& data) const
    {
        return to_quantity<Q>(data.value(), data.symbol());        
    }

    /**
     * Format a scalar/symbol pair into a dynamic quantity. Unlike the templated
     * version, this version searchs *all* maps for the symbol, using the first
     * formatter found. If the symbol is not in any maps, this returns a
     * bad_quantity.
     */
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

    /**
     * Format a scalar/symbol pair into a dynamic quantity. Unlike the templated
     * version, this version searchs *all* maps for the symbol, using the first
     * formatter found. If the symbol is not in any maps, this returns a
     * bad_quantity.
     */
    quantity_type to_quantity(formatted const& data) const
    {
        return to_quantity(data.value(), data.symbol());        
    }

    /**
     * Inspect the map for a given index. Do not delete this pointer.
     */
    map_type const* get(unit_type const& u) const
    {
        auto it = find(u);
        return (it != sorted_data.end() ? &(*it) : nullptr);
    }

    /**
     * Drop all maps.
     */
    void clear() { sorted_data.clear(); }

    /**
     * Get the number of maps contained in the group.
     */
    std::size_t size() const { return sorted_data.size(); }

  private:
    /**
     * Look up a may by index type.
     */
    const_iterator find(unit_type id) const
    {
        return detail::find(sorted_data, id, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Look up a may by index type.
     */
    iterator find(unit_type id) { return detail::find(sorted_data, id, compare_item_formatter, equal_item_formatter); }

    /**
     * Compare maps by index type. Used for sorting.
     */
    static bool compare_formatter(map_type const& left, map_type const& right) { return left.index() < right.index(); }

    /**
     * Compare a map to an index type. Used for searching.
     */
    static bool compare_item_formatter(map_type const& left, unit_type right) { return left.index() < right; }

    /**
     * Compare a map to an index type. Used for searching.
     */
    static bool equal_item_formatter(map_type const& left, unit_type right) { return left.index() == right; }

    /// input_format_maps sorted by index type
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
    /**
     * Format a quantity using the formatter for the unit type. If no formatter is available, return
     * a bad_format.
     */
    template <class Q, DIM_IS_QUANTITY(Q)> formatted format(Q const& q) const
    {
        auto it = find(::dim::index<Q>());
        return (it != sorted_data.end() ? it->template output<Q>(q) : formatted_quantity<scalar>::bad_format());
    }

    /**
     * Format a dynamic quantity using the formatter for the unit type. If no formatter is available, return
     * a bad_format.
     */
    formatted format(quantity_type const& q) const
    {
        auto it = find(q.unit());
        return (it != sorted_data.end() ? it->output(q) : formatted_quantity<scalar>::bad_format());
    }

    /**
     * Add or replace a formatter in the map for Q::unit.
     */
    template <class Q, DIM_IS_QUANTITY(Q)> bool insert(char const* s, Q const& scale, Q const& add = Q(0))
    {
        return insert(formatter_type(s, scale, add));
    }

    /**
     * Add or replace a formatter in the map for item.index().
     */
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

    /**
     * Get a pointer for the formatter for a given unit_type (or nullptr if not found).
     */
    formatter_type const* get(unit_type const& u) const
    {
        auto it = find(u);
        return (it != sorted_data.end() ? &(*it) : nullptr);
    }

    /**
     * Remove the formatter for a unit_type
     */
    bool erase(unit_type index)
    {
        auto it = find(index);
        if (it != sorted_data.end()) {
            sorted_data.erase(it);
            return true;
        }
        return false;
    }

    /**
     * Remove all formatters.
     */
    void clear() { sorted_data.clear(); }

    /**
     * Get the number of formatters in the map.
     */
    std::size_t size() const { return sorted_data.size(); }

  private:
    /**
     * Look up a unit_type in the map
     */
    const_iterator find(unit_type id) const
    {
        return detail::find(sorted_data, id, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Look up a unit_type in the map
     */
    iterator find(unit_type id) { return detail::find(sorted_data, id, compare_item_formatter, equal_item_formatter); }

    /**
     * Sorting function for the map
     */
    static bool compare_formatter(formatter_type const& left, formatter_type const& right)
    {
        return left.index() < right.index();
    }

    /**
     * Searching function for the map (used for bisection search).
     */
    static bool compare_item_formatter(formatter_type const& left, unit_type right) { return left.index() < right; }

    /**
     * Equality function for the map (used in final phase of search).
     */
    static bool equal_item_formatter(formatter_type const& left, unit_type right) { return left.index() == right; }

    /**
     * A list of formatters sorted by index.
     */
    std::vector<formatter_type> sorted_data;
};

template <class Q> using static_input_format_map = input_format_map<typename Q::scalar, typename Q::unit::system>;

/**
 * @brief Create template specializations of this function for your system to control the
 * default formats available for input
 */
template <class Q> input_format_map<typename Q::scalar, typename Q::system> const& get_default_format()
{
    input_format_map<typename Q::scalar, typename Q::system> static const EMPTY(::dim::index<Q>());
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
    auto dynamic_q = detail::parse_standard_rep<typename Q::scalar, typename Q::system>(formatted.symbol(), formatted.symbol() + kMaxSymbol);
    o_q = (formatted.value() * dynamic_q).template as<Q>();
    return !(o_q.is_bad());
}

/**
 * @brief Default input parsing for quantities.
 * Returns false if unit_str could not be understood.
 * Works in two phases:
 * (1) Look up unit_str in the map group
 * (2) If not found, use the system's dynamic quantity parser
 *
 * @note This version searches all maps in a map group to match a symbol.
 */
template <class Scalar, class System>
bool parse_quantity(dynamic_quantity<Scalar, System>& o_q, formatted_quantity<Scalar> const& formatted,
                    input_format_map_group<Scalar, System> const& unit_map)
{
    o_q = unit_map.to_quantity(formatted.value(), formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }
    o_q = formatted.value() * detail::parse_standard_rep<Scalar, System>(formatted.symbol(), formatted.symbol() + kMaxSymbol);
    return !o_q.is_bad();
}

/**
 * @brief Default input parsing for quantities.
 * Returns false if unit_str could not be understood.
 * Works in two phases:
 * (1) Look up unit_str in the map
 * (2) If not found, use the system's dynamic quantity parser
 *
 * @note This version searches only one map.
 */
template <class Scalar, class System>
bool parse_quantity(dynamic_quantity<Scalar, System>& o_q, formatted_quantity<Scalar> const& formatted,
                    input_format_map<Scalar, System> const& unit_map)
{
    o_q = unit_map.to_quantity(formatted.value(), formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }
    o_q = formatted.value() * detail::parse_standard_rep<Scalar, System>(formatted.symbol(), formatted.symbol() + kMaxSymbol);
    return !o_q.is_bad();
}

/**
 * Turn a quantity into a formatted_quantity. If out_map is not null, this map
 * is first used. If it is null or doesn't contain a matching formatter, we fall
 * back to using print_unit().
 */
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
    o_formatted = formatted_quantity<typename Q::scalar>(dimensionless_cast(i_q));
    print_unit(o_formatted.symbol(), o_formatted.symbol() + kMaxSymbol, i_q);
    return true;
}

/**
 * Turn a dynamic_quantity into a formatted_quantity. If out_map is not null,
 * this map is first used. If it is null or doesn't contain a matching
 * formatter, we fall back to using print_unit().
 */
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
    o_formatted = formatted_quantity<Scalar>(dimensionless_cast(i_q));
    print_unit(o_formatted.symbol(), o_formatted.symbol() + kMaxSymbol, i_q);
    return true;
}

} // namespace dim
