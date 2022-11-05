#include "dim/io_stream.hpp"
#include "si_io.hpp"

namespace dim {
namespace si {

/*
 * Install the default facet to the global locale
 * before main()
 */
namespace {
int internal_install_facet()
{
    add_to_global_locale();
    return 0;
}

const int __dummy = internal_install_facet();
    
}
    
void add_to_global_locale(quantity_facet* specialized) {
    std::locale base_locale; // Initialized to current global
    if (nullptr == specialized) {
        specialized = system::make_default_facet();
    }
    std::locale enhanced_locale(base_locale, specialized);
    std::locale::global(enhanced_locale);
    // Install the locale in the default streams
    std::cout.imbue(enhanced_locale);
    std::cerr.imbue(enhanced_locale);
    std::clog.imbue(enhanced_locale);
    std::cin.imbue(enhanced_locale);
}

quantity_facet* system::make_default_facet() {
    quantity_facet* facet = new quantity_facet();
    facet->input_formatter(get_default_format<Temperature>());
    facet->input_formatter(get_default_format<Length>());
    facet->input_formatter(get_default_format<Time>());
    facet->input_formatter(get_default_format<Mass>());
    facet->input_formatter(get_default_format<Angle>());
    facet->input_formatter(get_default_format<SolidAngle>());
    facet->input_formatter(get_default_format<Temperature>());
    facet->input_formatter(get_default_format<Amount>());
    facet->input_formatter(get_default_format<Current>());
    facet->input_formatter(get_default_format<Luminosity>());
    facet->input_formatter(get_default_format<Frequency>());
    facet->input_formatter(get_default_format<Force>());
    facet->input_formatter(get_default_format<Pressure>());
    facet->input_formatter(get_default_format<Energy>());
    facet->input_formatter(get_default_format<Power>());
    facet->input_formatter(get_default_format<Charge>());
    facet->input_formatter(get_default_format<Voltage>());
    facet->input_formatter(get_default_format<Capacitance>());
    facet->input_formatter(get_default_format<Resistance>());
    facet->input_formatter(get_default_format<Conductance>());
    facet->input_formatter(get_default_format<MagneticFlux>());
    facet->input_formatter(get_default_format<MagneticFluxDensity>());
    facet->input_formatter(get_default_format<Inductance>());
    facet->input_formatter(get_default_format<LuminousFlux>());
    facet->input_formatter(get_default_format<Luminance>());
    facet->input_formatter(get_default_format<CatalyticActivity>());
    facet->input_formatter(get_default_format<Area>());
    facet->input_formatter(get_default_format<Volume>());
    facet->input_formatter(get_default_format<FlowRate>());
    facet->input_formatter(get_default_format<Speed>());
    facet->input_formatter(get_default_format<Acceleration>());
    facet->input_formatter(get_default_format<AngularRate>());
    facet->input_formatter(get_default_format<AngularAcceleration>());
    facet->input_formatter(get_default_format<Torque>());
    facet->input_formatter(get_default_format<Density>());
    facet->input_formatter(get_default_format<Viscosity>());
    facet->input_formatter(get_default_format<KinematicViscosity>());

    facet->output_formatter(formatter<Length>("m", meter));
    facet->output_formatter(formatter<Time>("s", second));
    facet->output_formatter(formatter<Mass>("kg", kilogram));
    facet->output_formatter(formatter<Angle>("rad", radian));
    facet->output_formatter(formatter<SolidAngle>("sr", steradian));
    facet->output_formatter(formatter<Temperature>("K", kelvin));
    facet->output_formatter(formatter<Amount>("mol", mole));
    facet->output_formatter(formatter<Current>("A", ampere));
    facet->output_formatter(formatter<Luminosity>("cd", candela));
    facet->output_formatter(formatter<Frequency>("Hz", hertz));
    facet->output_formatter(formatter<Force>("N", newton));
    facet->output_formatter(formatter<Pressure>("Pa", pascal));
    facet->output_formatter(formatter<Energy>("J", joule));
    facet->output_formatter(formatter<Power>("W", watt));
    facet->output_formatter(formatter<Charge>("C", coulomb));
    facet->output_formatter(formatter<Voltage>("V", volt));
    facet->output_formatter(formatter<Capacitance>("F", farad));
    facet->output_formatter(formatter<Resistance>("R", ohm));
    facet->output_formatter(formatter<Conductance>("S", siemens));
    facet->output_formatter(formatter<MagneticFlux>("Wb", weber));
    facet->output_formatter(formatter<MagneticFluxDensity>("T", tesla));
    facet->output_formatter(formatter<Inductance>("H", henry));
    facet->output_formatter(formatter<LuminousFlux>("Im", lumen));
    facet->output_formatter(formatter<Luminance>("Ix", lux));
    facet->output_formatter(formatter<CatalyticActivity>("kat", katal));
    facet->output_formatter(formatter<Viscosity>("Pl", poiseuille));
    facet->output_formatter(formatter<Area>("m^2", meter2));
    facet->output_formatter(formatter<Volume>("L", liter));
    facet->output_formatter(formatter<FlowRate>("L/s", liter/second));
    facet->output_formatter(formatter<Speed>("m/s", meter/second));
    facet->output_formatter(formatter<Acceleration>("m/s^2", meter/second/second));
    facet->output_formatter(formatter<AngularRate>("rad/s", radian/second));
    facet->output_formatter(formatter<AngularAcceleration>("rad/s^2", radian/second/second));
    facet->output_formatter(formatter<Torque>("N*m/rad", newton*meter/radian));
    facet->output_formatter(formatter<Density>("kg/m^3", kilogram/meter3));
    facet->output_formatter(formatter<KinematicViscosity>("St", meter2/second));

    return facet;
}

}
}

