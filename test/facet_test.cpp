#include "doctest.h"
#include "dim/si.hpp"

TEST_CASE("facet")
{
    si::facet* fac = si::system::make_default_facet();
    fac->output_formatter("in", si::inch);
    si::system::install_facet(fac);
    std::locale loc;
    REQUIRE(std::has_facet<si::facet>(loc));
    REQUIRE(std::has_facet<si::facet>(std::cout.getloc()));
    REQUIRE(std::has_facet<si::facet>(std::cerr.getloc()));
    REQUIRE(std::has_facet<si::facet>(std::clog.getloc()));
    REQUIRE(std::has_facet<si::facet>(std::cin.getloc()));    

    si::dynamic_quantity dq(5.0, si::meter_);
    si::formatted_quantity fq = std::use_facet<si::facet>(loc).format(dq);
    CHECK(fq.value() == doctest::Approx(5.0 * si::meter/si::inch));
    CHECK_MESSAGE(strncmp(fq.symbol(), "in", 2) == 0, "Expected in, got ", std::string(fq.symbol()));

    dq = std::use_facet<si::facet>(loc).format(2.0, "in");
    CHECK(dimensionless_cast(dq) == doctest::Approx(2.0 * si::inch / si::meter));

    si::Length q = 5.0 * si::inch;
    fq = std::use_facet<si::facet>(loc).format(q);
    CHECK(fq.value() == doctest::Approx(5.0));
    CHECK_MESSAGE(strncmp(fq.symbol(), "in", 2) == 0, "Expected in, got ", std::string(fq.symbol()));

    q = std::use_facet<si::facet>(loc).format<si::Length>(2.0, "in");
    CHECK(dimensionless_cast(dq) == doctest::Approx(2.0 * si::inch / si::meter));
}
