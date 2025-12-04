#pragma once
#include "definition.hpp"

/**
 * Definitions of the SI formatters for each quantity
 */ 

namespace dim
{

namespace detail
{
/** 
 * @brief Parse unit strings using SI conventions.
 *
 * This is the fallback parser for when a unit symbol string isn't found in
 * get_default_format<>.
 * @param unit_str Start of unit string
 * @param end Pointer past the end of unit_str
 * @return dynamic_quantity result of the parse. This will be a bad_quantity()
 * if the parse was not successful.
 */
template <>::dim::si::dynamic_quantity parse_standard_rep<double, si::system>(char const* i_unit_str, char const* i_end);

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
