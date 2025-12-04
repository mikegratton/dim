#pragma once
#include "dim/io_detail.hpp"
#include "dim/tag.hpp"
#include "dynamic_quantity.hpp"
#include "io.hpp"
#include <algorithm>
#include <cstring>
#include <initializer_list>
#include <vector>

/**
 * format_maps are containers holding formatter objects. For input (turning a
 * string into a quantity), the maps are indexed by symbol c-string. These
 * input_maps are grouped together into an input_map_group, a map of maps.  The
 * input_map_group is keyed off of the dynamic_unit.  For output, the formatters
 * are keyed off of dynamic_unit, but there's only one formatter per unique
 * dynamic_unit.
 *
 * These maps are all implemented as sorted arrays, so insertion and deletion are
 * O(n log(n)), but search is O(log n).  (This is to improve performance during
 * look-up time at the expense of modification time.)
 */

namespace dim
{

namespace detail
{
/// Bisection search for an item. This uses an equality comparator to determine
/// if it has found the result.
template <class Container, class Key, class Compare, class Equal>
typename Container::const_iterator find(Container const& i_container, Key const& i_key, Compare const& i_comp,
                                        Equal const& i_equal)
{
    auto it = std::lower_bound(i_container.begin(), i_container.end(), i_key, i_comp);
    if (it == i_container.end() || !i_equal(*it, i_key)) {
        return i_container.end();
    }
    return it;
}

/// Bisection search for an item. This uses an equality comparator to determine
/// if it has found the result.
template <class Container, class Key, class Compare, class Equal>
typename Container::iterator find(Container& i_container, Key const& i_key, Compare const& i_comp, Equal const& i_equal)
{
    auto it = std::lower_bound(i_container.begin(), i_container.end(), i_key, i_comp);
    if (it == i_container.end() || !i_equal(*it, i_key)) {
        return i_container.end();
    }
    return it;
}
} // namespace detail

/**
 * @brief Format a scalar plus string to quantity or dynamic_quantity.
 *
 * Each of these is for a given quantity (denoted by its index aka dynamic_unit).
 * If a quantity doesn't match this index, this returns a bad quantity.
 */
template <class Scalar, class System>
class input_format_map
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
    explicit input_format_map(unit_type i_unit_code)
        : m_index(i_unit_code)
    {
    }

    /**
     * Create an empty map for the unit type given by U.
     */
    template <class U, DIM_IS_UNIT(U)>
    explicit input_format_map(U const& i_unit_code)
        : m_index(i_unit_code)
    {
    }

    /**
     * Create a map with the unit type given by first_formatter.index(). Insert
     * this formatter into the map.
     */
    explicit input_format_map(formatter_type i_first_formater)
        : m_index(i_first_formater.index())
    {
        insert(i_first_formater);
    }

    /**
     * Create a map with the given contents. The type will be set to that of the
     * first item. If there are no items, the type will be an invalid type.
     * Items with types not matching the first item will be rejected.
     */
    input_format_map(std::initializer_list<formatter_type> const& i_formatter_list)
        : m_index(i_formatter_list.size() > 0 ? formatter_type(*i_formatter_list.begin()).index() : unit_type::bad_unit())
    {
        for (auto const& item : i_formatter_list) {
            insert(item);
        }
    }

    /**
     * Add a formatter to the map. Q::unit must match the type of the map.
     * @return True if insertion occurred. False if the item has the wrong index
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    bool insert(char const* i_symbol, Q const& i_scale, Q const& i_add = Q(0))
    {
        return insert(formatter_type(i_symbol, i_scale, i_add));
    }

    /**
     * Add a formatter to the map. item.index() must match the type of the map.
     * @return True if insertion occurred. False if the item has the wrong index
     */
    bool insert(formatter_type const& i_item)
    {
        if (i_item.index() != index()) {
            return false;
        }
        auto it = find(i_item.symbol());
        if (it != m_sorted_data.end()) {
            *it = i_item;
            return true;
        }
        m_sorted_data.push_back(i_item);
        std::sort(m_sorted_data.begin(), m_sorted_data.end(), compare_formatter);
        return true;
    }

    /**
     * Transform the scalar/symbol pair into a quantity. If Q::unit is the wrong type, or symbol is not in the map,
     * return a bad_quantity().
     *
     * @note This form is not preferred. It can suffer buffer overruns.
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q to_quantity(typename Q::scalar const& i_scalar, char const* i_symbol) const
    {
        if (::dim::index<Q>() != index()) {
            return Q::bad_quantity();
        }
        auto it = find(i_symbol);
        return (it != m_sorted_data.end() ? it->template input<Q>(i_scalar) : Q::bad_quantity());
    }

    /**
     * Transform the scalar/symbol pair into a quantity. If Q::unit is the wrong type, or symbol is not in the map,
     * return a bad_quantity().
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q to_quantity(formatted_quantity<scalar> const& i_input) const
    {
        return to_quantity<Q>(i_input.value(), i_input.symbol());
    }

    /**
     * Transform the scalar/symbol pair into a dynamic_quantity. If the symbol is
     * not in the map, return a bad_quantity().
     *
     * @note This form is not preferred. It can suffer buffer overruns.
     */
    quantity_type to_quantity(Scalar const& i_scalar, char const* i_symbol) const
    {
        auto it = find(i_symbol);
        return (it != m_sorted_data.end() ? it->input(i_scalar) : quantity_type::bad_quantity());
    }

    /**
     * Transform the scalar/symbol pair into a quantity. If Q::unit is the wrong type, or symbol is not in the map,
     * return a bad_quantity().
     */
    quantity_type to_quantity(formatted_quantity<scalar> const& i_input) const
    {
        return to_quantity(i_input.value(), i_input.symbol());
    }

    /**
     * Get a pointer to a formatter by symbol type. If the symbol is unknown,
     * this returns a nullptr. Do not delete this pointer.
     */
    formatter_type const* get(char const* i_symbol) const
    {
        auto it = find(i_symbol);
        return (it != m_sorted_data.end() ? &(*it) : nullptr);
    }

    /**
     * Get the number of items in the map.
     */
    std::size_t size() const { return m_sorted_data.size(); }

    /**
     * Clear all items from the map.
     */
    void clear() { m_sorted_data.clear(); }

    /**
     * Obtain the unit type that this formatter handles.
     */
    unit_type index() const { return m_index; }

    /**
     * Remove a symbol from the map. Returns true if removal occured.
     */
    bool erase(char const* i_symbol)
    {
        auto it = find(i_symbol);
        if (it != m_sorted_data.end()) {
            m_sorted_data.erase(it);
            return true;
        }
        return false;
    }

  private:
    /**
     * Look up an entry by symbol
     */
    const_iterator find(char const* i_symbol) const
    {
        return detail::find(m_sorted_data, i_symbol, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Look up an entry by symbol
     */
    iterator find(char const* i_symbol)
    {
        return detail::find(m_sorted_data, i_symbol, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Compare formatters by symbol. For sorting the map.
     */
    static bool compare_formatter(formatter_type const& i_left, formatter_type const& i_right)
    {
        return strncmp(i_left.symbol(), i_right.symbol(), kMaxSymbol) < 0;
    }

    /**
     * Compare a formatter to a symbol. For bisection search.
     */
    static bool compare_item_formatter(formatter_type const& i_element, char const* i_query)
    {
        return strncmp(i_element.symbol(), i_query, kMaxSymbol) < 0;
    }

    /**
     * Compare a formatter to a symbol. For bisection search.
     */
    static bool equal_item_formatter(formatter_type const& i_element, char const* i_query)
    {
        return strncmp(i_element.symbol(), i_query, kMaxSymbol) == 0;
    }

    /// Unit type for this map
    unit_type m_index;

    /// Formatters sorted by symbol
    std::vector<formatter_type> m_sorted_data;
};

/**
 * @brief A group of input_format_maps organized by index.
 *
 * For each index, there's an input_format_map of formatters representing
 * different symbols for that quantity.
 */
template <class Scalar, class System>
class input_format_map_group
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
    template <class Q, DIM_IS_QUANTITY(Q)>
    bool insert(char const* i_symbol, Q const& i_scale, Q const& i_add = Q(0))
    {
        return insert(formatter_type(i_symbol, i_scale, i_add));
    }

    /**
     * Insert a formatter into the map group. If a map for index<Q>() exists,
     * this formatter is added to that group. Otherwise a new map is created and
     * the formatter is added to the new map.
     */
    bool insert(formatter_type const& i_item)
    {
        iterator it = find(i_item.index());
        if (it != m_sorted_data.end()) {
            return it->insert(i_item);
        }
        map_type new_map(i_item.index());
        new_map.insert(i_item);
        m_sorted_data.push_back(new_map);
        std::sort(m_sorted_data.begin(), m_sorted_data.end(), compare_formatter);
        return true;
    }

    /**
     * Insert a map into the map group. Any existing map for the index() type is
     * destroyed.
     */
    bool insert(map_type const& i_whole_map)
    {
        auto it = find(i_whole_map.index());
        if (it != m_sorted_data.end()) {
            *it = i_whole_map;
            return true;
        }
        m_sorted_data.push_back(i_whole_map);
        std::sort(m_sorted_data.begin(), m_sorted_data.end(), compare_formatter);
        return true;
    }

    /**
     * Erase a map by index type. Returns true if a map was removed
     */
    bool erase(unit_type i_index)
    {
        auto it = find(i_index);
        if (it != m_sorted_data.end()) {
            m_sorted_data.erase(it);
            return true;
        }
        return false;
    }

    /**
     * Erase a map entry by index type and symbol. If a formatter matching the index
     * and symbol is found, it is removed and the function returns true.
     */
    bool erase(unit_type i_index, char const* i_symbol)
    {
        auto it = find(i_index);
        if (it != m_sorted_data.end()) {
            bool status = it->erase(i_symbol);
            if (it->size() == 0) {
                m_sorted_data.erase(it);
            }
            return status;
        }
        return false;
    }

    /**
     * Format a scalar/symbol pair into a quantity of type Q. If no map for
     * index<Q>() exists, or there is no matching symbol in the map, this returns
     * a bad_quantity.
     *
     * @note This form is not preferred. It can suffer buffer overruns.
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q to_quantity(typename Q::scalar const& i_scalar, char const* i_symbol) const
    {
        auto it = find(::dim::index<Q>());
        return (it != m_sorted_data.end() ? it->template to_quantity<Q>(i_scalar, i_symbol) : Q::bad_quantity());
    }

    /**
     * Format a scalar/symbol pair into a quantity of type Q. If no map for
     * index<Q>() exists, or there is no matching symbol in the map, this returns
     * a bad_quantity.
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q to_quantity(formatted const& i_data) const
    {
        return to_quantity<Q>(i_data.value(), i_data.symbol());
    }

    /**
     * Format a scalar/symbol pair into a dynamic quantity. Unlike the templated
     * version, this version searchs *all* maps for the symbol, using the first
     * formatter found. If the symbol is not in any maps, this returns a
     * bad_quantity.
     *
     * @note This form is not preferred. It can suffer buffer overruns.
     */
    quantity_type to_quantity(Scalar const& i_scalar, char const* i_symbol) const
    {
        for (auto const& map : m_sorted_data) {
            quantity_type value = map.to_quantity(i_scalar, i_symbol);
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
    quantity_type to_quantity(formatted const& i_input) const { return to_quantity(i_input.value(), i_input.symbol()); }

    /**
     * Inspect the map for a given index. Do not delete this pointer.
     */
    map_type const* get(unit_type const& i_index) const
    {
        auto it = find(i_index);
        return (it != m_sorted_data.end() ? &(*it) : nullptr);
    }

    /**
     * Drop all maps.
     */
    void clear() { m_sorted_data.clear(); }

    /**
     * Get the number of maps contained in the group.
     */
    std::size_t size() const { return m_sorted_data.size(); }

  private:
    /**
     * Look up a may by index type.
     */
    const_iterator find(unit_type i_index) const
    {
        return detail::find(m_sorted_data, i_index, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Look up a may by index type.
     */
    iterator find(unit_type i_index) { return detail::find(m_sorted_data, i_index, compare_item_formatter, equal_item_formatter); }

    /**
     * Compare maps by index type. Used for sorting.
     */
    static bool compare_formatter(map_type const& i_left, map_type const& i_right) { return i_left.index() < i_right.index(); }

    /**
     * Compare a map to an index type. Used for searching.
     */
    static bool compare_item_formatter(map_type const& i_element, unit_type i_query) { return i_element.index() < i_query; }

    /**
     * Compare a map to an index type. Used for searching.
     */
    static bool equal_item_formatter(map_type const& i_element, unit_type i_query) { return i_element.index() == i_query; }

    /// input_format_maps sorted by index type
    std::vector<map_type> m_sorted_data;
};

/**
 * @brief A map of formatters to turn quantities into (scalar, string) pairs.
 *
 * Formatters are organized by a quantity's index. Each dynamic_unit
 * may have one formatter.
 */
template <class Scalar, class System>
class output_format_map
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
    template <class Q, DIM_IS_QUANTITY(Q)>
    formatted format(Q const& q) const
    {
        auto it = find(::dim::index<Q>());
        return (it != m_sorted_data.end() ? it->template output<Q>(q) : formatted_quantity<scalar>::bad_format());
    }

    /**
     * Format a dynamic quantity using the formatter for the unit type. If no formatter is available, return
     * a bad_format.
     */
    formatted format(quantity_type const& q) const
    {
        auto it = find(q.unit());
        return (it != m_sorted_data.end() ? it->output(q) : formatted_quantity<scalar>::bad_format());
    }

    /**
     * Add or replace a formatter in the map for Q::unit.
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    bool insert(char const* i_symbol, Q const& i_scale, Q const& i_add = Q(0))
    {
        return insert(formatter_type(i_symbol, i_scale, i_add));
    }

    /**
     * Add or replace a formatter in the map for item.index().
     */
    bool insert(formatter_type const& i_item)
    {
        auto it = find(i_item.index());
        if (it != m_sorted_data.end()) {
            *it = i_item;
            return true;
        }
        m_sorted_data.push_back(i_item);
        std::sort(m_sorted_data.begin(), m_sorted_data.end(), compare_formatter);
        return true;
    }

    /**
     * Get a pointer for the formatter for a given unit_type (or nullptr if not found).
     */
    formatter_type const* get(unit_type const& u) const
    {
        auto it = find(u);
        return (it != m_sorted_data.end() ? &(*it) : nullptr);
    }

    /**
     * Remove the formatter for a unit_type
     */
    bool erase(unit_type i_index)
    {
        auto it = find(i_index);
        if (it != m_sorted_data.end()) {
            m_sorted_data.erase(it);
            return true;
        }
        return false;
    }

    /**
     * Remove all formatters.
     */
    void clear() { m_sorted_data.clear(); }

    /**
     * Get the number of formatters in the map.
     */
    std::size_t size() const { return m_sorted_data.size(); }

  private:
    /**
     * Look up a unit_type in the map
     */
    const_iterator find(unit_type id) const
    {
        return detail::find(m_sorted_data, id, compare_item_formatter, equal_item_formatter);
    }

    /**
     * Look up a unit_type in the map
     */
    iterator find(unit_type id) { return detail::find(m_sorted_data, id, compare_item_formatter, equal_item_formatter); }

    /**
     * Sorting function for the map
     */
    static bool compare_formatter(formatter_type const& i_left, formatter_type const& i_right)
    {
        return i_left.index() < i_right.index();
    }

    /**
     * Searching function for the map (used for bisection search).
     */
    static bool compare_item_formatter(formatter_type const& i_element, unit_type i_query) { return i_element.index() < i_query; }

    /**
     * Equality function for the map (used in final phase of search).
     */
    static bool equal_item_formatter(formatter_type const& i_element, unit_type i_query) { return i_element.index() == i_query; }

    /**
     * A list of formatters sorted by index.
     */
    std::vector<formatter_type> m_sorted_data;
};


/**
 * @brief Create template specializations of this function for your system to control the
 * default formats available for input
 */
template <class Q>
input_format_map<typename Q::scalar, typename Q::system> const& get_default_format()
{
    input_format_map<typename Q::scalar, typename Q::system> static const EMPTY(::dim::index<Q>());
    return EMPTY;
}

/**
 * @brief Default input parsing for quantities. Returns false if unit_str is
 * wrong for Q. This is the recommended parser entrypoint.
 *
 * Works in two phases: (1) Look up unit_str in unit_map (backed by the
 * default_format map if not specified) (2) If not found, use Q::system's
 * dynamic quantity parser
 */
template <class Q, DIM_IS_QUANTITY(Q)>
bool parse_quantity(Q& o_q, formatted_quantity<typename Q::scalar> const& i_formatted,
                    input_format_map<typename Q::scalar, typename Q::system> const& i_unit_map = get_default_format<Q>())
{
    o_q = i_unit_map.template to_quantity<Q>(i_formatted.value(), i_formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }
    auto dynamic_q = detail::parse_standard_rep<typename Q::scalar, typename Q::system>(
        i_formatted.symbol(), i_formatted.symbol() + kMaxSymbol);
    o_q = (i_formatted.value() * dynamic_q).template as<Q>();
    return !(o_q.is_bad());
}

/**
 * @brief Default input parsing for dynamic_quantities. Returns false if
 * unit_str could not be understood. This is the recommended parser entrypoint.
 *
 * Works in two phases: (1) Look up unit_str in the map group (2) If not found,
 * use the system's dynamic quantity parser
 *
 * @note This version searches all maps in a map group to match a symbol.
 */
template<class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
bool parse_quantity(DQ& o_q, formatted_quantity<typename DQ::scalar> const& i_formatted,
                    input_format_map_group<typename DQ::scalar, typename DQ::system> const& i_unit_map)
{
    o_q = i_unit_map.to_quantity(i_formatted.value(), i_formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }
    o_q = i_formatted.value() *
          detail::parse_standard_rep<typename DQ::scalar, typename DQ::system>(i_formatted.symbol(), i_formatted.symbol() + kMaxSymbol);
    return !o_q.is_bad();
}

/**
 * @brief Default input parsing for quantities. Returns false if unit_str could
 * not be understood. This is the recommended parser entrypoint.
 *
 * Works in two phases: (1) Look up unit_str in the map (2) If not found, use
 * the system's dynamic quantity parser
 *
 * @note This version searches only one map. It is more efficient, provided you
 * know the quantity type.
 */
template<class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
bool parse_quantity(DQ& o_q, formatted_quantity<typename DQ::scalar> const& i_formatted,
                    input_format_map<typename DQ::scalar, typename DQ::system> const& i_unit_map)
{
    o_q = i_unit_map.to_quantity(i_formatted.value(), i_formatted.symbol());
    if (!o_q.is_bad()) {
        return true;
    }
    o_q = i_formatted.value() *
          detail::parse_standard_rep<typename DQ::scalar, typename DQ::system>(i_formatted.symbol(), i_formatted.symbol() + kMaxSymbol);
    return !o_q.is_bad();
}

/**
 * @brief Turn a quantity into a formatted_quantity. 
 *
 * If out_map is not null, this map is first used.  This is the recommended
 * serialization entrypoint. If the map pointer is null or doesn't contain a
 * matching formatter, we fall back to using print_unit().
 */
template <class Q, DIM_IS_QUANTITY(Q)>
bool format_quantity(formatted_quantity<typename Q::scalar>& o_formatted, Q const& i_q,
                     output_format_map<typename Q::scalar, typename Q::system> const* i_out_map = nullptr)
{
    if (i_out_map) {
        o_formatted = i_out_map->format(i_q);
        if (!o_formatted.is_bad()) {
            return true;
        }
    }
    o_formatted = formatted_quantity<typename Q::scalar>(dimensionless_cast(i_q));
    print_unit(o_formatted.symbol(), o_formatted.symbol() + kMaxSymbol, i_q);
    return true;
}

/**
 * @brief Turn a dynamic_quantity into a formatted_quantity.
 *
 * If out_map is not null, this map is first used. If it is null or doesn't
 * contain a matching formatter, we fall back to using print_unit(). This is the
 * recommended serialization entrypoint.
 */
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
bool format_quantity(formatted_quantity<typename DQ::scalar>& o_formatted, DQ const& i_q,
                     output_format_map<typename DQ::scalar, typename DQ::system> const* i_out_map = nullptr)
{
    using scalar = typename DQ::scalar;
    if (i_out_map) {
        o_formatted = i_out_map->format(i_q);
        if (!o_formatted.is_bad()) {
            return true;
        }
    }
    o_formatted = formatted_quantity<scalar>(dimensionless_cast(i_q));
    print_unit(o_formatted.symbol(), o_formatted.symbol() + kMaxSymbol, i_q);
    return true;
}

} // namespace dim
