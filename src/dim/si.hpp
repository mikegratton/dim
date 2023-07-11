#pragma once
#ifndef DIM_NO_STREAM
#include "io_stream.hpp"
#endif
#include "dim/si/definition.hpp"
#include "dim/si/literal.hpp"
#include "dim/si/si_io.hpp"

/**
 * @brief Simplify the namespace so that all symbols are in the namespace `si`.
 */
namespace si = dim::si;

namespace dim {
namespace si {
using namespace ::dim;
}
}  // namespace dim
