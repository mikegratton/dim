#pragma once
#include "base.hpp"

namespace dim 
{
namespace detail
{

template<class System>
void print_helper (char** out_symbol, int a, int dimension, bool& space)
{
    if (a == 0)
        return;
    if (space)
        *(*out_symbol)++ = '_';
    int advance;
    if (a == 1)
        advance = sprintf (*out_symbol, "%s", System::symbol_for (dimension));
    else
        advance = sprintf (*out_symbol, "%s^%d", System::symbol_for (dimension), a);
    (*out_symbol) = *out_symbol + advance;
    space = true;
}

template<class U>
void print_unit (char* out_symbol, unit_base<U> const&, bool& spaceit)
{
    using System = typename U::system;
    const char* sym = System::template specialized_symbol<U>();
    if (sym[0]) {
        if (spaceit)
            sprintf (out_symbol, "_%s", sym);
        else
            sprintf (out_symbol, "%s", sym);
        return;
    }
    char** tmp_symbol = &out_symbol;
    print_helper<System> (tmp_symbol, U::angle(),         base_dimension::Angle,       spaceit);    
    print_helper<System> (tmp_symbol, U::mass(),          base_dimension::Mass,        spaceit);
    print_helper<System> (tmp_symbol, U::length(),        base_dimension::Length,      spaceit);
    print_helper<System> (tmp_symbol, U::temperature(),   base_dimension::Temperature, spaceit);
    print_helper<System> (tmp_symbol, U::amount(),        base_dimension::Amount,      spaceit);
    print_helper<System> (tmp_symbol, U::current(),       base_dimension::Current,     spaceit);
    print_helper<System> (tmp_symbol, U::luminosity(),    base_dimension::Luminosity,  spaceit);
    print_helper<System> (tmp_symbol, U::time(),          base_dimension::Time,        spaceit);
}

struct char_less {
    bool operator() (const char* const& c1, const char* const& c2) const { return (std::strcmp(c1, c2) < 0); }
};

// Tag type for generic format map containers
struct container_base { virtual ~container_base() { } };


template<class System, class scalar=double>
dynamic_quantity<scalar, System> parse_standard_rep(const char* unit_str, int nend=-1)
{
   return dynamic_quantity<scalar, System>::bad();
}

/*
 * Parse unit_str to a dynamic_quantity. If the dynamic_unit doesn't match U's
 * dimensions, return false.  Otherwise "scale" represents the transform to 
 * U's system
 */
template<class U, class scalar=double, DIM_IS_UNIT(U)>
bool parse_unit_dynamic(scalar& scale, const char* unit_str)
{
   dynamic_quantity<scalar, typename U::system> dim = parse_standard_rep<typename U::system, scalar>(unit_str);
   scale = dim.value;
   return unitsMatch(dim.unit, toDynamic<U>());   
}

} // end of namespace detail

} // end of namespace dim
