#include "si_facet.hpp"
#include "definition.hpp"
#include "si_io.hpp"
#include <iostream>

/**
 * The facet contains the formatters for SI.
 */

namespace dim {

template class dynamic_quantity<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
template class formatter<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
template class input_format_map<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
template class input_format_map_group<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
template class output_format_map<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;
template class quantity_facet<typename si::dynamic_quantity::scalar, typename si::dynamic_quantity::system>;

namespace si {

/// The mandatory public static id member of the facet.
std::locale::id facet::id;


void install_facet(si::facet* i_quantity_facet)
{
    std::locale base_locale;  // Initialized to current global    
    if (!i_quantity_facet) {
        i_quantity_facet = make_default_facet();
    }
    std::locale enhanced_locale(base_locale, i_quantity_facet);
    std::locale::global(enhanced_locale);
    // Install the locale in the default streams
    std::cout.imbue(enhanced_locale);
    std::cerr.imbue(enhanced_locale);
    std::clog.imbue(enhanced_locale);
    std::cin.imbue(enhanced_locale);
}


facet* make_default_facet()
{
    facet* instance = new facet();
    instance->input_formatter(get_default_format<Acceleration>());
    instance->input_formatter(get_default_format<Amount>());
    instance->input_formatter(get_default_format<Angle>());
    instance->input_formatter(get_default_format<AngularAcceleration>());
    instance->input_formatter(get_default_format<AngularRate>());
    instance->input_formatter(get_default_format<Area>());
    instance->input_formatter(get_default_format<Capacitance>());
    instance->input_formatter(get_default_format<CatalyticActivity>());
    instance->input_formatter(get_default_format<Charge>());
    instance->input_formatter(get_default_format<Conductance>());
    instance->input_formatter(get_default_format<Current>());
    instance->input_formatter(get_default_format<Density>());
    instance->input_formatter(get_default_format<Energy>());
    instance->input_formatter(get_default_format<FlowRate>());   
    instance->input_formatter(get_default_format<Force>());
    instance->input_formatter(get_default_format<Frequency>());
    instance->input_formatter(get_default_format<Inductance>());
    instance->input_formatter(get_default_format<KinematicViscosity>());
    instance->input_formatter(get_default_format<Length>());
    instance->input_formatter(get_default_format<Luminance>());
    instance->input_formatter(get_default_format<Luminosity>());
    instance->input_formatter(get_default_format<LuminousFlux>());
    instance->input_formatter(get_default_format<MagneticFlux>());
    instance->input_formatter(get_default_format<MagneticFluxDensity>());
    instance->input_formatter(get_default_format<Mass>());
    instance->input_formatter(get_default_format<Power>());
    instance->input_formatter(get_default_format<Pressure>());
    instance->input_formatter(get_default_format<Resistance>());
    instance->input_formatter(get_default_format<SolidAngle>());
    instance->input_formatter(get_default_format<Speed>());
    instance->input_formatter(get_default_format<Temperature>());
    instance->input_formatter(get_default_format<Time>());
    instance->input_formatter(get_default_format<Torque>());
    instance->input_formatter(get_default_format<Viscosity>());      
    instance->input_formatter(get_default_format<Voltage>());
    instance->input_formatter(get_default_format<Volume>());
      
    instance->output_formatter("m", meter);
    instance->output_formatter("s", second);
    instance->output_formatter("kg", kilogram);
    instance->output_formatter("rad", radian);
    instance->output_formatter("sr", steradian);
    instance->output_formatter("K", kelvin);
    instance->output_formatter("mol", mole);
    instance->output_formatter("A", ampere);
    instance->output_formatter("cd", candela);
    instance->output_formatter("Hz", hertz);
    instance->output_formatter("N", newton);
    instance->output_formatter("Pa", pascal);
    instance->output_formatter("J", joule);
    instance->output_formatter("W", watt);
    instance->output_formatter("C", coulomb);
    instance->output_formatter("V", volt);
    instance->output_formatter("F", farad);
    instance->output_formatter("Ω", ohm);
    instance->output_formatter("S", siemens);
    instance->output_formatter("Wb", weber);
    instance->output_formatter("T", tesla);
    instance->output_formatter("H", henry);
    instance->output_formatter("Im", lumen);
    instance->output_formatter("Ix", lux);
    instance->output_formatter("kat", katal);
    instance->output_formatter("Pl", poiseuille);
    instance->output_formatter("m^2", meter2);
    instance->output_formatter("L", liter);
    instance->output_formatter("L/s", liter / second);
    instance->output_formatter("m/s", meter / second);
    instance->output_formatter("m/s^2", meter / second / second);
    instance->output_formatter("rad/s", radian / second);
    instance->output_formatter("rad/s^2", radian / second / second);
    instance->output_formatter("N*m/rad", newton * meter / radian);
    instance->output_formatter("kg/m^3", kilogram / meter3);
    instance->output_formatter("St", meter2 / second);

    return instance;
}

}  // namespace si
}  // namespace dim
