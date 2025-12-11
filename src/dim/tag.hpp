#pragma once
#include "DimConfig.hpp"
#include <type_traits>

#if __cplusplus < 201103L
#error "C++11 or higher is required for dim"
#endif

#if __cplusplus < 201402L
namespace std
{
template< bool B, class T = void >
using enable_if_t = typename ::std::enable_if<B,T>::type;    
}
#endif

namespace dim
{

/// Tag type for units
struct unit_tag {};

/// Tag type for quantities
struct quantity_tag {};

/// Tag type for dynamic_unit
struct dynamic_unit_tag {};

/// Tag type for dynamic_quantity
struct dynamic_quantity_tag {};

/// Tag type for systems
struct system_tag {};



// SFINAE macros that can be used as template parameters

/// Use as a template parameter to check if TYPE has been tagged TAG
#define DIM_IS_TAGGED_FOR(TAG, TYPE) typename std::enable_if_t<std::is_base_of<TAG, TYPE>::value>* = nullptr

/// Use as a template parameter to check if U is a unit
#define DIM_IS_UNIT(U) DIM_IS_TAGGED_FOR(::dim::unit_tag, U)

/// Use as a template parameter to check if Q is a quantity
#define DIM_IS_QUANTITY(Q) DIM_IS_TAGGED_FOR(::dim::quantity_tag, Q)

/// Use as a template parameter to check if Q is a quantity
#define DIM_IS_DYNAMIC_UNIT(U) DIM_IS_TAGGED_FOR(::dim::dynamic_unit_tag, U)

/// Use as a template parameter to check if Q is a quantity
#define DIM_IS_DYNAMIC_QUANTITY(Q) DIM_IS_TAGGED_FOR(::dim::dynamic_quantity_tag, Q)

/// Use as a template parameter to check if T is a system
#define DIM_IS_SYSTEM(T) DIM_IS_TAGGED_FOR(::dim::system_tag, T)

/// Check if S is a scalar type (float, double, etc)
#define DIM_IS_SCALAR(S) typename std::enable_if_t<std::is_arithmetic<S>::value>* = nullptr

// Macros for dimension list
#define DIM_ARRAY                                                                                                      \
    int8_t Length, int8_t Time, int8_t Mass, int8_t Angle, int8_t Temperature, int8_t Amount, int8_t Current,          \
        int8_t Luminosity
#define DIM_D_ARRAY Length, Time, Mass, Angle, Temperature, Amount, Current, Luminosity

/// Convenience enum for accessing dynamic_quantity dimensions
enum class base_dimension { DIM_D_ARRAY };

} // namespace dim