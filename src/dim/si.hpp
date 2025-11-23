#pragma once
#ifdef DIM_NO_STREAM
#include "si_no_stream.hpp"
#else
#include "io_stream.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/literal.hpp"
#include "dim/si/si_io.hpp"
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
