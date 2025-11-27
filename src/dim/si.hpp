#pragma once
#ifndef DIM_NO_STREAM
#include "iostream.hpp"
#endif
#include "si_no_stream.hpp"
#include "dim/ioformat.hpp"

/**
 * @brief Simplify the namespace so that all symbols are in the namespace `si`.
 */
namespace si = dim::si;

namespace dim {
namespace si {
using namespace ::dim;
}
}  // namespace dim
