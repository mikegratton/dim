#include "format_map.hpp"

namespace dim {

void format_index_map::set(long index_, std::unique_ptr<detail::container_base> payload_)
{
    auto* found = (Node*)bsearch(&index_, map, mapSize, sizeof(Node), map_compare);
    if (found) {
        found->payload = std::move(payload_);
        return;
    }
    if (mapSize == mapCapacity) {
        mapCapacity *= 2;
        Node* newMap = new Node[mapCapacity];
        for (int i = 0; i < mapSize; i++) {
            newMap[i].index = map[i].index;
            newMap[i].payload = std::move(map[i].payload);
        }
        delete[] map;
        map = newMap;
    }
    map[mapSize].index = index_;
    map[mapSize].payload = std::move(payload_);
    mapSize++;
    qsort(map, mapSize, sizeof(Node), map_sort);
}

detail::container_base const* format_index_map::get(long index) const
{
    auto const* found = static_cast<Node const*>(bsearch(&index, map, mapSize, sizeof(Node), map_compare));
    if (found) { return found->payload.get(); }
    return nullptr;
}

detail::container_base* format_index_map::get(long index)
{
    auto* found = static_cast<Node*>(bsearch(&index, map, mapSize, sizeof(Node), map_compare));
    if (found) { return found->payload.get(); }
    return nullptr;
}

void format_index_map::clear()
{
    mapSize = 0;
    delete[] map;
    map = new Node[mapCapacity];
}

int format_index_map::map_compare(void const* vkey, void const* velement)
{
    long const* key = (long const*)vkey;
    Node const* element = (Node const*)velement;
    return *key - element->index;
}

int format_index_map::map_sort(void const* velement1, void const* velement2)
{
    Node const* element1 = (Node const*)velement1;
    Node const* element2 = (Node const*)velement2;
    return element1->index - element2->index;
}

}  // namespace dim