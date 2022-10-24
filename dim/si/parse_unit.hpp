#pragma once
#include "definition.hpp"
#include "dim/io.hpp"

namespace dim 
{
namespace si
{
using dynamic_quantity = ::dim::dynamic_quantity<double, si::system>;

namespace detail
{
dynamic_quantity parse_known_quantity(const char* buf);
double find_si_prefix(int& start, const char* buf);
}
}
}
