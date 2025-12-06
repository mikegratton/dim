# Migrating from Version 1

* The static `quantity` class template is essentially unchanged. Quantity
  arithmetic works as before. 
* The `quantity::index()` and `unit::index()` methods have been removed. Use
  `index<Q>().raw()` or `index<U>().raw()` instead. Consider using
  `si::dynamic_unit` instead of raw `uint64_t` index values.
* The old `dynamic_unit` typedef has been replaced with a fully-featured
  `dynamic_unit` class. This class acts as the old index value; it is a
  comparable type that is used as a key in the format maps.
* The `formatter<>` and `dynamic_formatter` have been merged into a type-erased
  `formatter` type. This type has constructors that take the same arguments as
  the old class. 
* The format map classes are now 
    * `input_format_map`, for storing formatters for the same unit indexed by symbol string.
    * `input_format_map_group`, for storing `input_format_map` objects for many unit types indexed by `dynamic_unit`
    * `output_format_map`, for storing formatters indexed by unit type (one per unit type)
* New typedefs in the `si::` namespace have been provided to ease operations:
    * `si::formatted_quantity`
    * `si::formatter`
    * `si::input_format_map`
    * `si::input_format_map_group`
    * `si::output_format_map`
    * `si::dynamic_unit`
    * `si::dynamic_quantity`

## Constructing the New Formatter Types
```cpp
si::output_format_map myOutputMap;
si::formatter fancyFormat("°/s", si::degree / si::second);
myOutputMap.insert(fancyFormat); // Insert existing formatters into the map
myOutputMap.insert("°", si::degree); // You can construct formatters directly this way

si::input_format_map_group myInputMap;
myInputMap.insert("furlong", 220.0 * si::yard); // Detects which input_format_map to insert this formatter
```