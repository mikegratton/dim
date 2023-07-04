#include "dim/format_map.hpp"

#include <iostream>
#include <string>

#include "dim/io.hpp"
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