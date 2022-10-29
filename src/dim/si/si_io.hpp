#pragma once
#include "dim/io.hpp"
#include "definition.hpp"

namespace dim {
namespace si {
    using dynamic_quantity = ::dim::dynamic_quantity<double, si::system>;
}
// Parse unit strings using si conventions
// This is defined in lexer quantity.l
namespace detail {
template<>
::dim::si::dynamic_quantity parse_standard_rep<si::system, double>(const char* unit_str, int nend);
}

template<>
void print_unit<dim::si::system>(char* buf, dynamic_unit const& unit, bool spaceit);

template<>
format_map<si::Length> const& get_default_format<si::Length>();
template<>
format_map<si::Time> const& get_default_format<si::Time>();
template<>
format_map<si::Mass> const& get_default_format<si::Mass>();
template<>
format_map<si::Angle> const& get_default_format<si::Angle>();
template<>
format_map<si::SolidAngle> const& get_default_format<si::SolidAngle>();
template<>
format_map<si::Temperature> const& get_default_format<si::Temperature>(); 

template<>
format_map<si::Force> const& get_default_format<si::Force>();
template<>
format_map<si::Pressure> const& get_default_format<si::Pressure>();
template<>
format_map<si::Energy> const& get_default_format<si::Energy>();
template<>
format_map<si::Power> const& get_default_format<si::Power>();

template<>
format_map<si::Area> const& get_default_format<si::Area>();
template<>
format_map<si::Volume> const& get_default_format<si::Volume>();
template<>
format_map<si::FlowRate> const& get_default_format<si::FlowRate>();
template<>
format_map<si::Speed> const& get_default_format<si::Speed>();
template<>
format_map<si::Acceleration> const& get_default_format<si::Acceleration>();
template<>
format_map<si::AngularRate> const& get_default_format<si::AngularRate>();
template<>
format_map<si::AngularAcceleration> const& get_default_format<si::AngularAcceleration>();
template<>
format_map<si::Torque> const& get_default_format<si::Torque>();

}
