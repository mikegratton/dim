#include "dim/si.hpp"
#include "doctest.h"

TEST_CASE("CompileError")
{
    si::Length L = 5.0*si::meter;
    auto A = L*L*L + L*L;
    si::Area A2 = L;
}
