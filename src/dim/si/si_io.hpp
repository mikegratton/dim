#pragma once
#include "definition.hpp"

namespace dim
{

namespace detail
{
/// Parse unit strings using SI conventions
template <>::dim::si::dynamic_quantity parse_standard_rep<double, si::system>(char const* unit_str, int nend);

} // namespace detail


template <> si::input_format_map const& get_default_format<si::Length>();
template <> si::input_format_map const& get_default_format<si::Time>();
template <> si::input_format_map const& get_default_format<si::Mass>();
template <> si::input_format_map const& get_default_format<si::Angle>();
template <> si::input_format_map const& get_default_format<si::SolidAngle>();
template <> si::input_format_map const& get_default_format<si::Temperature>();

template <> si::input_format_map const& get_default_format<si::Force>();
template <> si::input_format_map const& get_default_format<si::Pressure>();
template <> si::input_format_map const& get_default_format<si::Energy>();
template <> si::input_format_map const& get_default_format<si::Power>();

template <> si::input_format_map const& get_default_format<si::Area>();
template <> si::input_format_map const& get_default_format<si::Volume>();
template <> si::input_format_map const& get_default_format<si::FlowRate>();
template <> si::input_format_map const& get_default_format<si::Speed>();
template <> si::input_format_map const& get_default_format<si::Acceleration>();
template <> si::input_format_map const& get_default_format<si::AngularRate>();
template <> si::input_format_map const& get_default_format<si::AngularAcceleration>();
template <> si::input_format_map const& get_default_format<si::Torque>();

} // namespace dim
