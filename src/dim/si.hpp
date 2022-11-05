#pragma once
#ifndef DIM_NO_STREAM
#include "io_stream.hpp"
#endif
#include "dim/si/definition.hpp"
#include "dim/si/literal.hpp"
#include "dim/si/si_io.hpp"

/*
 * Simplify the namespaces for si. All symbols are in the SI namespace.
 */
namespace si = dim::si;

namespace dim {
namespace si
{
    using namespace ::dim;
}
}
