#include "definition.hpp"
#include "dim/si/si_facet.hpp"

namespace dim {
namespace si {
const char* system::kSymbol[] = {"m", "s", "kg", "rad", "K", "mol", "A", "cd"};
const long system::id = 26980L;

static si::output_format_map specialized_symbol_map;

void system::set_specialized_symbol(dynamic_unit const& u, char const* symbol)
{
    si::formatter special_format(symbol, dynamic_quantity(u));
    specialized_symbol_map.insert(special_format);
}

static void initialize_specialized_symbol_map()
{
    system::set_specialized_symbol(::dim::index<Length>(), "m");
    system::set_specialized_symbol(::dim::index<Time>(), "s");
    system::set_specialized_symbol(::dim::index<Mass>(), "kg");
    system::set_specialized_symbol(::dim::index<Angle>(), "rad");
    system::set_specialized_symbol(::dim::index<Temperature>(), "K");
    system::set_specialized_symbol(::dim::index<Amount>(), "mol");
    system::set_specialized_symbol(::dim::index<Current>(), "A");
    system::set_specialized_symbol(::dim::index<Luminosity>(), "cd");

    system::set_specialized_symbol(::dim::index<Frequency>(), "Hz");
    system::set_specialized_symbol(::dim::index<SolidAngle>(), "sr");
    system::set_specialized_symbol(::dim::index<Force>(), "N");
    system::set_specialized_symbol(::dim::index<Pressure>(), "Pa");
    system::set_specialized_symbol(::dim::index<Energy>(), "J");
    system::set_specialized_symbol(::dim::index<Power>(), "W");
    system::set_specialized_symbol(::dim::index<Charge>(), "C");
    system::set_specialized_symbol(::dim::index<Voltage>(), "V");
    system::set_specialized_symbol(::dim::index<Capacitance>(), "F");
    system::set_specialized_symbol(::dim::index<Resistance>(), "R");
    system::set_specialized_symbol(::dim::index<Conductance>(), "S");
    system::set_specialized_symbol(::dim::index<MagneticFlux>(), "Wb");
    system::set_specialized_symbol(::dim::index<MagneticFluxDensity>(), "T");
    system::set_specialized_symbol(::dim::index<Inductance>(), "H");
    system::set_specialized_symbol(::dim::index<LuminousFlux>(), "Im");
    system::set_specialized_symbol(::dim::index<Luminance>(), "Ix");
    system::set_specialized_symbol(::dim::index<CatalyticActivity>(), "kat");
    system::set_specialized_symbol(::dim::index<Viscosity>(), "Pl");
}

const char* system::specialized_symbol(dynamic_unit const& u)
{
    static bool s_init_map = true;
    if (s_init_map) {
        initialize_specialized_symbol_map();
    }
    auto const* special = specialized_symbol_map.get(u);
    return (special? special->symbol() : nullptr);    
}

}  // namespace si

template class dynamic_unit<si::system>;

}  // namespace dim
