#include "dim/dynamic_quantity.hpp"
#include "dim/format_map.hpp"

#include <iostream>
#include <string>
#include "dim/io_detail.hpp"
#include "dim/si.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/si_io.hpp"
#include "dim/si/si_quantity_facet.hpp"
#include "doctest.h"

TEST_CASE("input_format_map")
{
    using LengthMap = si::input_format_map;    
    LengthMap map1(si::formatter("in", si::inch));        
    auto result = map1.to_quantity(5.0, "in");
    CHECK(dimensionless_cast(result / si::dynamic_quantity(si::inch)) == doctest::Approx(5.0));
    CHECK(dimensionless_cast(result / si::dynamic_quantity(si::inch)) == doctest::Approx(5.0));
    auto result2 = map1.to_quantity<si::Length>(5.0, "in");
    CHECK(result2 / si::inch == doctest::Approx(5.0));

    // Symbol replacement    
    map1.insert("in", si::yard);    
    result = map1.to_quantity(5.0, "in");
    CHECK(result.value() == doctest::Approx(15.0 * si::foot / si::meter));

    // Bulk init
    LengthMap map2{{"in", si::inch}, {"yd", si::yard}, {"mi", si::mile}, {"nmi", si::nautical_mile}};
    CHECK(map2.size() == 4);
    CHECK(dimensionless_cast(map2.to_quantity(2.0, "mi")) == doctest::Approx(dimensionless_cast(2.0 * si::mile)));
    CHECK(dimensionless_cast(map2.to_quantity(2.0, "nmi")) == doctest::Approx(dimensionless_cast(2.0 * si::nautical_mile)));
    CHECK(dimensionless_cast(map2.to_quantity(2.0, "yd")) == doctest::Approx(dimensionless_cast(2.0 * si::yard)));
    CHECK(dimensionless_cast(map2.to_quantity(2.0, "in")) == doctest::Approx(dimensionless_cast(2.0 * si::inch)));    

    // Clear the map
    map2.clear();
    CHECK(map2.size() == 0);
}

static std::string print(si::dynamic_unit unit)
{
    std::string rep = "[";
    for (int i=0; i<unit.size(); i++) {
        rep += std::to_string(static_cast<int>(unit.get(i))) + " ";
    }
    rep += "]";
    return rep;
}

TEST_CASE("input_format_map_group")
{
    si::input_format_map_group imap;
    // Fill the map by inserting input_format_maps 
    imap.insert(si::get_default_format<si::Length>());
    imap.insert(si::get_default_format<si::Temperature>());
    imap.insert(si::get_default_format<si::Time>());
    imap.insert(si::get_default_format<si::Mass>());
    CHECK(imap.size() == 4);

    // Erase a whole unit class
    CHECK(imap.erase(dim::index<si::Length>()));
    CHECK(imap.size() == 3);

    // Insert a formatter for a new unit
    imap.insert(si::formatter("in", si::inch));
    CHECK(imap.size() == 4);
    imap.insert(si::formatter("yd", si::yard));

    // ... and take it out again
    CHECK(imap.erase(dim::index<si::Length>(), "in"));
    CHECK(imap.size() == 4);
    // When the last element is removed, check that the whole length map was removed
    CHECK(imap.erase(dim::index<si::Length>(), "yd"));
    CHECK(imap.size() == 3);

    // Check we can access the formatters
    auto result = imap.to_quantity(5.0, "hr");
    CHECK_FALSE(result.is_bad());    
    CHECK(result.unit() == dim::index<si::Time>());    
    CHECK(dimensionless_cast(result) == doctest::Approx(5.0 * si::hour / si::second));
    

    // Access with a specified quantity
    auto result2 = imap.to_quantity<si::Mass>(5.0, "lbm");
    CHECK(index(result2) == dim::index<si::Mass>());
    CHECK_FALSE(result2.is_bad());
    CHECK(dimensionless_cast(result2) == doctest::Approx(5.0 * si::pound_mass / si::kilogram));

    // Try to access a non-existant format for dynamic types
    auto result3 = imap.to_quantity(3.0, "smoot");
    CHECK(result3.is_bad());

    // Try to access the wrong quantity
    auto result4 = imap.to_quantity<si::Mass>(4.0, "hr");
    CHECK(result4.is_bad());

    // Clear the map
    imap.clear();
    CHECK(imap.size() == 0);
}

TEST_CASE("output_format_map")
{
    si::output_format_map omap;
    omap.insert(si::formatter("hr", si::hour));
    omap.insert("smoot", 1.7018 * si::meter);
    CHECK(omap.size() == 2);

    // Replace a type
    omap.insert(si::formatter("min", si::minute));
    CHECK(omap.size() == 2);

    // Format a static type
    auto result = omap.format(2.0 * si::meter);
    CHECK_FALSE(dim::isbad__(result.value()));
    CHECK(strncmp(result.symbol(), "smoot", dim::kMaxSymbol) == 0);
    CHECK(result.value() == doctest::Approx(2.0 / 1.7018));

    // Format a dynamic type 
    result = omap.format(si::dynamic_quantity(2.0, dim::index<si::Length>()));
    CHECK_FALSE(dim::isbad__(result.value()));
    CHECK(strncmp(result.symbol(), "smoot", dim::kMaxSymbol) == 0);
    CHECK(result.value() == doctest::Approx(2.0 / 1.7018));

    // Clear the map
    omap.clear();
    CHECK(omap.size() == 0);
}

TEST_CASE("quantity_index")
{
    CHECK(dim::index<si::Frequency>() != dim::index<si::AngularRate>());
}

TEST_CASE("parse_quantity")
{
    si::input_format_map input_format(si::meter_);
    input_format.insert("yd", si::yard);
    si::dynamic_quantity dq;
    CHECK(parse_quantity(dq, 2.0, "yd", input_format));
    CHECK(dimensionless_cast(dq) == doctest::Approx(2.0 * si::yard / si::meter));
    si::Length q;
    CHECK(parse_quantity(q, 2.0, "yd", input_format));
    CHECK(dimensionless_cast(q) == doctest::Approx(2.0 * si::yard / si::meter));
}