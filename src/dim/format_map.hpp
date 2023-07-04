#pragma once
#include <cstdlib>
#include <cstring>
#include <initializer_list>
#include <memory>

#include "dim/base.hpp"
#include "dim/io_detail.hpp"
#include "io.hpp"

namespace dim {

template <class SymbolFormat>
class format_symbol_map : public detail::container_base {
   protected:
   public:
    format_symbol_map() : mapCapacity(8), mapSize(0), map(new SymbolFormat[mapCapacity]) {}
    ~format_symbol_map() { delete[] map; }
    format_symbol_map(format_symbol_map const& other) { *this = other; }

    format_symbol_map(std::initializer_list<SymbolFormat> init)
        : mapCapacity(init.size()), mapSize(init.size()), map(new SymbolFormat[mapCapacity])
    {
        unsigned long i = 0;
        for (auto const& format : init) { memcpy(&map[i++], &format, sizeof(SymbolFormat)); }
        qsort(map, mapSize, sizeof(SymbolFormat), map_sort);
    }

    format_symbol_map& operator=(format_symbol_map const& other)
    {
        delete[] map;
        mapSize = other.mapSize;
        mapCapacity = other.mapCapacity;
        map = new SymbolFormat[mapCapacity];
        memcpy(map, other.map, sizeof(SymbolFormat) * mapSize);
        return *this;
    }

    template <class Q, DIM_IS_QUANTITY(Q)>
    void set(const char* symbol_, Q const& scale_, Q const& add_ = Q(0))
    {
        set(SymbolFormat(symbol_, scale_, add_));
    }

    void set(SymbolFormat format)
    {
        auto* found = (SymbolFormat*)bsearch(format.symbol(), map, mapSize, sizeof(SymbolFormat), map_compare);
        if (found) {
            memcpy(found, &format, sizeof(SymbolFormat));
            return;
        }
        if (mapSize == mapCapacity) {
            mapCapacity *= 2;
            SymbolFormat* newMap = new SymbolFormat[mapCapacity];
            memcpy(newMap, map, sizeof(SymbolFormat) * mapSize);
            delete[] map;
            map = newMap;
        }
        memcpy(&map[mapSize], &format, sizeof(SymbolFormat));
        mapSize++;
        qsort(map, mapSize, sizeof(SymbolFormat), map_sort);
    }

    SymbolFormat const* get(char const* symbol) const
    {
        return static_cast<SymbolFormat const*>(bsearch(symbol, map, mapSize, sizeof(SymbolFormat), map_compare));
    }

    unsigned long size() const { return mapSize; }

   protected:
    static int map_compare(void const* vkey, void const* velement)
    {
        char const* key = (char const*)vkey;
        SymbolFormat const* element = (SymbolFormat const*)velement;
        return strncmp(key, element->symbol(), kMaxSymbol);
    }

    static int map_sort(void const* velement1, void const* velement2)
    {
        SymbolFormat const* element1 = (SymbolFormat const*)velement1;
        SymbolFormat const* element2 = (SymbolFormat const*)velement2;
        return strncmp(element1->symbol(), element2->symbol(), kMaxSymbol);
    }

    unsigned long mapCapacity;
    unsigned long mapSize;
    SymbolFormat* map;
};

class format_index_map : public detail::container_base {
   protected:
    struct Node {
        long index;
        std::unique_ptr<detail::container_base> payload;
    };

   public:
    format_index_map() : mapCapacity(8), mapSize(0), map(new Node[mapCapacity]) {}
    ~format_index_map() { delete[] map; }
    format_index_map(format_index_map const& other) { *this = other; }

    void set(long index_, std::unique_ptr<detail::container_base> payload_);

    detail::container_base const* get(long index) const;

    detail::container_base* get(long index);

    unsigned long size() const { return mapSize; }

    void clear();

   protected:
    static int map_compare(void const* vkey, void const* velement);
    static int map_sort(void const* velement1, void const* velement2);

    unsigned long mapCapacity;
    unsigned long mapSize;
    Node* map;
};

template <class Q, DIM_IS_QUANTITY(Q)>
using format_map = format_symbol_map<formatter<Q>>;

/**
 * @brief Create template specializations of this function for your system to control the
 * default formats available for input
 */
template <class Q, DIM_IS_QUANTITY(Q)>
format_map<Q> const& get_default_format()
{
    static const format_map<Q> EMPTY;
    return EMPTY;
}

/// Map from char to a formatter object (for input)
template <class S, class System>
using dynamic_format_map = format_symbol_map<dynamic_formatter<S, System>>;

/**
 * @brief Default input parsing for quantities.
 * Returns false if unit_str is wrong for Q.
 * Works in two phases:
 * (1) Look up unit_str in unit_map (backed by the default_format map if not specified)
 * (2) If not found, use Q::system's dynamic quantity parser
 */
template <class Q, DIM_IS_QUANTITY(Q)>
bool parse_quantity(Q& o_q, double value, const char* unit_str, format_map<Q> const& unit_map = get_default_format<Q>())
{
    auto const* formatter = unit_map.get(unit_str);
    if (formatter) {
        o_q = formatter->input(value);
        return true;
    }
    using scalar = typename Q::scalar;
    scalar scale;
    if (detail::parse_unit_dynamic<typename Q::unit, scalar>(scale, unit_str)) {
        o_q = Q(value * scale);
        return true;
    }
    o_q = Q::bad_quantity();
    return false;
}

}  // namespace dim
