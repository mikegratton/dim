#pragma once
#include <string>
#include "dim/si.hpp"

/// Fallback dead-simple print out of a dynamic unit
std::string to_string(si::dynamic_unit const& u);