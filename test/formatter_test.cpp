#include "dim/incommensurable_exception.hpp"
#include "dim/io_detail.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/si_facet.hpp"
#include "doctest.h"
#include "dim/si.hpp"


TEST_CASE("formatter")
{
    auto f = si::formatter("in", dim::si::inch);
    CHECK(f.index() == dim::index(si::meter_));
    CHECK(strcmp(f.symbol(), "in") == 0);

    auto l = 2.0 * dim::si::meter;
    double in_inch = f.non_dim(l);
    CHECK(l / dim::si::inch == doctest::Approx(in_inch));
    auto ff = f.output(l);
    CHECK(ff.value() == doctest::Approx(in_inch));
    CHECK(strcmp(ff.symbol(), "in") == 0);
    sprintf(ff.symbol(), "%s", "moose");
    CHECK(strcmp(ff.symbol(), "moose") == 0);
    dim::si::Length l2 = f.input<si::Length>(in_inch);
    CHECK(dimensionless_cast(l2) == doctest::Approx(dimensionless_cast(l)));

    si::dynamic_quantity dq(3.0, si::meter_);
    auto dqf = f.output(dq);
    CHECK(dqf.value() == doctest::Approx(3.0 * si::meter/si::inch));
    CHECK(strcmp(dqf.symbol(), "in") == 0);    

    dq = f.input(5.0);
    CHECK(dq.value() == doctest::Approx(5.0 * si::inch/si::meter));
    CHECK(dq.unit() == f.index());

    si::Length q = f.input<si::Length>(5.0);
    CHECK(dimensionless_cast(q) == doctest::Approx(5.0 * si::inch/si::meter));

    si ::formatted_quantity fq;
    si::Temperature temp = si::fahrenheit(2.0);
    format_quantity(fq, temp);
    CHECK(fq.value() == temp/si::kelvin);
    
#ifdef DIM_EXCEPTIONS
    CHECK_THROWS_AS(f.input<si::Time>(1.0), dim::incommensurable_exception);
    CHECK_THROWS_AS(f.output(si::second), dim::incommensurable_exception);
    CHECK_THROWS_AS(f.output(si::dynamic_quantity(si::second)), dim::incommensurable_exception);
    CHECK_THROWS_AS(si::formatter("blah", si::yard, si::second), dim::incommensurable_exception);
#else
    si::Time t = f.input<si::Time>(1.0);
    CHECK(t.is_bad());

    CHECK(f.output(si::second).is_bad());
    dq = si::dynamic_quantity(si::second);
    CHECK(f.output(dq).is_bad());

    f = si::formatter("blah", si::yard, si::second);
    CHECK(std::string(f.symbol()) == "INCONSISTENT");
    CHECK(std::isnan(f.non_dim(si::meter)));
#endif
}

TEST_CASE("formatted_quantity")
{
    si::formatted_quantity fq(1.0, "01234567890123456789012345678901");
    std::string symbolString = fq.symbol();
    CHECK(symbolString.size() == dim::kMaxSymbol - 1);
    CHECK_MESSAGE(symbolString.back() == '0', "Expected 0, found ", symbolString.back());
    CHECK(fq.value() == 1.0);

    fq = si::formatted_quantity();
    CHECK(fq.is_bad());
    CHECK(strlen(fq.symbol()) == 0);
    fq.value(2.0);
    sprintf(fq.symbol(), "moose");
    CHECK(std::string(fq.symbol()) == "moose");
}