#pragma once
#include "dim/io.hpp"
#include "dim/quantity_facet.hpp"

namespace dim
{

namespace si
{

struct system;

using dynamic_quantity = ::dim::dynamic_quantity<double, si::system>;
using formatter = dim::formatter<typename dynamic_quantity::scalar, typename dynamic_quantity::system>;
using formatted_quantity = dim::formatted_quantity<typename dynamic_quantity::scalar>;
using input_format_map = ::dim::input_format_map<typename dynamic_quantity::scalar, typename dynamic_quantity::system>;
using input_format_map_group = ::dim::input_format_map_group<typename dynamic_quantity::scalar, typename dynamic_quantity::system>;
using output_format_map = ::dim::output_format_map<typename dynamic_quantity::scalar, typename dynamic_quantity::system>;

class facet : public quantity_facet<typename dynamic_quantity::scalar, typename dynamic_quantity::system>
{
  public:
    static std::locale::id id;
};

/**
 * @brief Obtain the default quantity_facet for si.
 */
facet* make_default_facet();

/**
 * @brief Install the default quantity facet into the global locale. Imbue
 * cout, cerr, and clog with it.
 */
void install_facet(facet* fac = make_default_facet());
} // namespace si

extern template class dynamic_quantity<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
extern template class formatter<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
extern template class input_format_map<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
extern template class input_format_map_group<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
extern template class output_format_map<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;

} // namespace dim