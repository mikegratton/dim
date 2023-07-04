#include "dim/format_map.hpp"

#include <iostream>
#include <string>

#include "dim/io.hpp"
#include "dim/io_detail.hpp"
#include "dim/si.hpp"
#include "dim/si/definition.hpp"
#include "doctest.h"

TEST_CASE("format_map")
{
    using LengthMap = dim::format_map<si::Length>;
    LengthMap map1, map2;
    map1.set(dim::formatter<si::Length>("in", si::inch));
    si::Length ell(5);
    CHECK(map1.get("in") != nullptr);
    auto result = map1.get("in")->output(ell);
    CHECK(result.value() > 196.84);

    // Symbol replacement
    map1.set("in", si::yard);
    CHECK(map1.get("in") != nullptr);
    result = map1.get("in")->output(ell);
    CHECK(result.value() < 5.47);

    // Map resize
    const char* static_text = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < 12; i++) { map1.set(dim::formatter<si::Length>(&static_text[i], si::mile)); }
    CHECK(map1.size() == 13);
    CHECK(map1.get("in") != nullptr);
    result = map1.get("in")->output(ell);
    CHECK(result.value() < 5.47);

    // Map copy
    map2 = map1;
    CHECK(map2.size() == 13);
    CHECK(map2.get("in") != nullptr);
    result = map2.get("in")->output(ell);
    CHECK(result.value() < 5.47);

    // Bulk init
    LengthMap map3{{"in", si::inch}, {"yd", si::yard}, {"mi", si::mile}, {"nmi", si::nautical_mile}};
    CHECK(map3.get("yd") != nullptr);
    CHECK(map3.get("in") != nullptr);
    CHECK(map3.get("mi") != nullptr);
    CHECK(map3.get("nmi") != nullptr);
}

TEST_CASE("format_index_map")
{
    dim::format_index_map imap;
    std::unique_ptr<dim::format_map<dim::si::Length>> lengthMap(new dim::format_map<dim::si::Length>);
    *lengthMap = si::get_default_format<si::Length>();
    imap.set(dim::formatter<si::Length>::index(), std::move(lengthMap));
    std::unique_ptr<dim::format_map<si::Temperature>> temperatureMap(new dim::format_map<si::Temperature>);
    *temperatureMap = si::get_default_format<si::Temperature>();
    imap.set(dim::formatter<si::Temperature>::index(), std::move(temperatureMap));
    std::unique_ptr<dim::format_map<si::Time>> timeMap(new dim::format_map<si::Time>);
    *timeMap = si::get_default_format<si::Time>();
    imap.set(dim::formatter<si::Time>::index(), std::move(timeMap));
    std::unique_ptr<dim::format_map<si::Mass>> massMap(new dim::format_map<si::Mass>);
    *massMap = si::get_default_format<si::Mass>();
    imap.set(dim::formatter<si::Mass>::index(), std::move(massMap));

    auto* m = static_cast<dim::format_map<si::Length>*>(imap.get(dim::formatter<si::Length>::index()));
    REQUIRE(m != nullptr);
    CHECK(m->size() == si::get_default_format<si::Length>().size());
    auto* m2 = static_cast<dim::format_map<si::Time>*>(imap.get(dim::formatter<si::Time>::index()));
    REQUIRE(m2 != nullptr);
    CHECK(m2->size() == si::get_default_format<si::Time>().size());
    auto* m3 = static_cast<dim::format_map<si::Temperature>*>(imap.get(dim::formatter<si::Temperature>::index()));
    REQUIRE(m3 != nullptr);
    CHECK(m3->size() == si::get_default_format<si::Temperature>().size());
    auto* m4 = static_cast<dim::format_map<si::Mass>*>(imap.get(dim::formatter<si::Mass>::index()));
    REQUIRE(m4 != nullptr);
    CHECK(m4->size() == si::get_default_format<si::Mass>().size());

    imap.erase(si::Mass::index());
    m4 = static_cast<dim::format_map<si::Mass>*>(imap.get(dim::formatter<si::Mass>::index()));
    CHECK(m4 == nullptr);
}

template <class Q>
void add_format(dim::format_index_map& io_map, char const* symbol, Q scale, Q add = Q(0.0))
{
    std::unique_ptr<dim::formatter<Q>> f(new dim::formatter<Q>(symbol, scale, add));
    io_map.set(Q::index(), std::move(f));
}

TEST_CASE("format_index_map2")
{
    dim::format_index_map imap;
    add_format(imap, "in", si::inch);
    add_format(imap, "hr", si::hour);
    add_format(imap, "K", si::kelvin);
    add_format(imap, "deg", si::degree);
    CHECK(imap.size() == 4);
    auto const* f = static_cast<dim::formatter<si::Time> const*>(imap.get(si::Time::index()));
    REQUIRE(f != nullptr);
    CHECK(std::string("hr") == f->symbol());
}

TEST_CASE("index")
{
    CHECK(si::Frequency::index() != si::AngularRate::index());
}