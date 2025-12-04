#pragma once
#include "config.hpp"
#include "si/definition.hpp"
#include "si/literal.hpp"
#include "si/si_io.hpp"
#include "ioformat.hpp"
#ifdef DIM_STREAM
#include "iostream.hpp"
#endif


/**
 * @brief Simplify the namespace so that all symbols are in the namespace `si`.
 */
namespace si = dim::si;

namespace dim {
namespace si {
using namespace ::dim;
}
}  // namespace dim
