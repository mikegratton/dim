#include "test_utilities.hpp"

std::string to_string(si::dynamic_unit const& u)
{
    std::string result = "[";
    for (int i = 0 ; i <u.size(); i++) {
        result += std::to_string(static_cast<int>(u.get(i))) + " ";
    }
    result += "]";
    return result;
}