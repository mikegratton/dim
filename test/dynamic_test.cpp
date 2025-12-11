#include "dim/incommensurable_exception.hpp"
#include "dim/io_detail.hpp"
#include "dim/si/definition.hpp"
#include "dim/si/si_facet.hpp"
#include "dim/unit.hpp"
#include "doctest.h"
#include "dim/si.hpp"
#include <cstdint>
#include <bitset>
#include <iostream>
#include "test_utilities.hpp"

namespace dim
{
template<class S, class T>
doctest::String toString(dynamic_quantity<S, T> const& value) {
    doctest::String s = doctest::toString(value.value()) + "_";
    char buf[256];
    detail::print_unit(buf, buf + sizeof(buf), nullptr, value.unit());
    s += buf;
    return s;
}
}

TEST_CASE("dynamic_unit.named")
{
    // init list
    si::dynamic_unit unit{1,2,3,4, -1,-2,-3,-4};
    // Getters
    CHECK(unit.length() == 1);
    CHECK(unit.time() == 2);
    CHECK(unit.mass() == 3);
    CHECK(unit.angle() == 4);
    CHECK(unit.temperature() == -1);
    CHECK(unit.amount() == -2);
    CHECK(unit.current() == -3);
    CHECK(unit.luminosity() == -4);
}

TEST_CASE("dynamic_unit.inverse") 
{
    si::dynamic_unit unit{1,2,3,4, -1,-2,-3,-4};
    // Inverse
    auto inv = inverse(unit);
    CHECK(inv.length() == -1);
    CHECK(inv.time() == -2);
    CHECK(inv.mass() == -3);
    CHECK(inv.angle() == -4);
    CHECK(inv.temperature() == 1);
    CHECK(inv.amount() == 2);
    CHECK(inv.current() == 3);
    CHECK(inv.luminosity() == 4);

    // Multiply
    auto result = unit.multiply(inv);
    CHECK(result.length() == 0);
    CHECK(result.time() == 0);
    CHECK(result.mass() == 0);
    CHECK(result.angle() == 0);
    CHECK(result.temperature() == 0);
    CHECK(result.amount() == 0);
    CHECK(result.current() == 0);
    CHECK(result.luminosity() == 0);
    // Dimensionless & equality
    CHECK(result == si::dynamic_unit::dimensionless());
}

TEST_CASE("dynamic_unit.setter") {
    // Setters
    si::dynamic_unit unit{1,2,3,4, -1,-2,-3,-4};
    si::dynamic_unit result(0L);
    result.length(1);
    result.time(2);
    result.mass(3);
    result.angle(4);
    result.temperature(-1);
    result.amount(-2);
    result.current(-3);
    result.luminosity(-4);
    CHECK(result == unit);
}

TEST_CASE("dynamic_unit.size") {    
    CHECK(si::dynamic_unit::size() == 8);
}

TEST_CASE("dynamic_unit.raw") {
    // raw/int init
    si::dynamic_unit unit{1,2,3,4, -1,-2,-3,-4};
    si::dynamic_unit u3(unit.raw());
    CHECK(u3 == unit);
}

TEST_CASE("dynamic_unit.get") 
{
    si::dynamic_unit unit{1,2,3,4, -1,-2,-3,-4};    
    CHECK(unit.length() == unit.get(0));
    CHECK(unit.time() == unit.get(1));
    CHECK(unit.mass() == unit.get(2));
    CHECK(unit.angle() == unit.get(3));
    CHECK(unit.temperature() == unit.get(4));
    CHECK(unit.amount() == unit.get(5));
    CHECK(unit.current() == unit.get(6));
    CHECK(unit.luminosity() == unit.get(7));
}

TEST_CASE("dynamic_unit.from") 
{
    CHECK(si::dynamic_unit(1, -1, 0, 0, 0, 0, 0, 0) == si::dynamic_unit::template from<typename si::Speed::unit>());   
}

TEST_CASE("dynamic_unit.comparison")
{
    si::dynamic_unit u1(0L);
    si::dynamic_unit u2(1L);
    si::dynamic_unit u3(1L);
    si::dynamic_unit u4(5L);
    CHECK(u1 < u2);
    CHECK(u1 <= u2);
    CHECK(u2 == u3);
    CHECK(u2 <= u3);
    CHECK(u2 >= u3);
    CHECK(u4 > u3);
    CHECK(u1 != u2);
            
    CHECK(!(u2 < u1));
    CHECK(!(u2 <= u1));
    CHECK(!(u1 == u2));
    CHECK(!(u1 > u2));
    CHECK(!(u1 >= u2));
    CHECK(!(u2 != u2));
}

TEST_CASE("dynamic_quantity.ctor")
{
    si::dynamic_quantity q(5.0, si::dynamic_unit(1L));
    CHECK(q.value() == 5.0);
    CHECK(q.unit() == dim::index<si::Length>());

    si::dynamic_quantity r(2.0);
    CHECK(r.dimensionless());
    CHECK(r.value() == 2.0);

    si::dynamic_quantity s(dim::index<si::Time>());
    CHECK(s.value() == 1.0);
    CHECK(s.unit() == dim::index<si::Time>());
    CHECK_FALSE(s.dimensionless());

    si::Speed speed(8.0);
    si::dynamic_quantity t(speed);
    CHECK(t.value() == 8.0);
    CHECK(t.unit() == dim::index(speed));
    CHECK(speed == t.template as<si::Speed>());

    si::dynamic_quantity u;
    CHECK(u.value() == 0);
    CHECK(u.dimensionless());
}

TEST_CASE("dynamic_quantity.setter")
{
    si::dynamic_quantity q(5.0, si::dynamic_unit(1L));
    q.value(7.0);
    CHECK(q.value() == 7.0);
    q.unit(dim::index<si::Mass>());
    CHECK(q.unit() == dim::index<si::Mass>());
}

TEST_CASE("dynamic_quantity.bad")
{
    si::dynamic_quantity q = si::dynamic_quantity::bad_quantity();
    CHECK(q.is_bad());
}

TEST_CASE("dynamic_quantity.operators")
{
    // quantity/quantity operators
    si::dynamic_quantity q(5.0, si::dynamic_unit(1L));
    si::dynamic_quantity s(2.0, dim::index<si::Time>());
    auto result = q*s;
    CHECK(result.value() == doctest::Approx(10.0));
    CHECK(result.unit() == si::dynamic_unit(1, 1, 0, 0, 0, 0, 0 ,0));

    result = q/s;
    CHECK(result.value() == doctest::Approx(2.5));
    CHECK(result.unit() == si::dynamic_unit(1, -1, 0, 0, 0, 0, 0 ,0));

    result = q;
    result *= s;
    CHECK(result.value() == doctest::Approx(10.0));
    CHECK(result.unit() == si::dynamic_unit(1, 1, 0, 0, 0, 0, 0 ,0));

    result = q;
    result /= s;
    CHECK(result.value() == doctest::Approx(2.5));
    CHECK(result.unit() == si::dynamic_unit(1, -1, 0, 0, 0, 0, 0 ,0));
    
    result = q + q;
    CHECK(result.value() == doctest::Approx(10.0));
    CHECK(result.unit() == q.unit());

    result = q - q;
    CHECK(result.value() == doctest::Approx(0.0));
    CHECK(result.unit() == q.unit());

    result = q;
    result += q;
    CHECK(result.value() == doctest::Approx(10.0));
    CHECK(result.unit() == q.unit());

    result = q;
    result -= q;
    CHECK(result.value() == doctest::Approx(0.0));
    CHECK(result.unit() == q.unit());

#ifdef DIM_EXCEPTIONS    
    CHECK_THROWS_AS(q + s, dim::incommensurable_exception);
    CHECK_THROWS_AS(q - s, dim::incommensurable_exception);
    si::dynamic_quantity t = q;
    CHECK_THROWS_AS(t += s, dim::incommensurable_exception);
    t = q;
    CHECK_THROWS_AS(t -= s, dim::incommensurable_exception);
#else
    result = q + s;
    CHECK(result.is_bad());
    result = q - s;
    CHECK(result.is_bad());
    result = q;
    result += s;
    CHECK(result.is_bad());
    result = q;
    result -= s;
    CHECK(result.is_bad());
#endif

    CHECK(q == q);
    CHECK(q <= q);
    CHECK(q >= q);
    auto q2 = 2.0 * q;
    CHECK(q < q2);
    CHECK(q2 > q);
    CHECK(q <= q2);
    CHECK(q2 >= q);
    CHECK(q != q2);
}

TEST_CASE("dynamic_quanity.scalar")
{
    // quantity/scalar operators
    si::dynamic_quantity q(5.0, si::dynamic_unit(1L));
    
    auto result = q * 2;    
    CHECK( result.unit() == q.unit());
    CHECK( result.value() == doctest::Approx(10.0));

    result = 2 * q;
    CHECK( result.unit() == q.unit());
    CHECK( result.value() == doctest::Approx(10.0));

    result = q / 2;
    CHECK( result.unit() == q.unit());
    CHECK( result.value() == doctest::Approx(2.5));

    result = 2 / q;
    CHECK( result.unit() == inverse(q.unit()));
    CHECK( result.value() == doctest::Approx(0.4));

    #ifdef DIM_EXCEPTIONS
    CHECK_THROWS_AS(2 + q, dim::incommensurable_exception);
    CHECK_THROWS_AS(q + 2, dim::incommensurable_exception);
    CHECK_THROWS_AS(2 - q, dim::incommensurable_exception);
    CHECK_THROWS_AS(q - 2, dim::incommensurable_exception);
    #else
    result = 2 + q;
    CHECK(result.is_bad());

    result = q + 2;
    CHECK(result.is_bad());

    result = 2 - q;
    CHECK(result.is_bad());

    result = q + 2;
    CHECK(result.is_bad());
    #endif

    si::dynamic_quantity dimless(2.0);
    
    result = 2 + dimless;
    CHECK(result.dimensionless());
    CHECK(result.value() == 4.0);

    result = dimless + 2;
    CHECK(result.dimensionless());
    CHECK(result.value() == 4.0);
        
    result = 2 - dimless;
    CHECK(result.dimensionless());
    CHECK(result.value() == 0);

    result = dimless - 2;
    CHECK(result.dimensionless());
    CHECK(result.value() == 0);

    result = dimless;
    result += 2;
    CHECK(result.dimensionless());
    CHECK(result.value() == 4.0);

    result = dimless;
    result -= 2;
    CHECK(result.dimensionless());
    CHECK(result.value() == 0);

}

TEST_CASE("dynamic_quanity.unit")
{
    // quantity/unit operators
    si::dynamic_quantity q(5.0, si::dynamic_unit(1L));
    si::dynamic_unit u1(q.unit());
    si::dynamic_unit u2(si::Speed::unit{});
    
    auto result = q * u1;
    CHECK( result.unit() == pow(q.unit(), 2));
    CHECK( result.value() == doctest::Approx(5.0));

    result = u1 * q;
    CHECK( result.unit() == pow(q.unit(), 2));
    CHECK( result.value() == doctest::Approx(5.0));

    result = q / u1;
    CHECK( result.dimensionless() );
    CHECK( result.value() == doctest::Approx(5.0));

    result = u1 / q;
    CHECK( result.dimensionless());
    CHECK( result.value() == doctest::Approx(0.2));

#ifdef DIM_EXCEPTIONS
    CHECK_THROWS_AS(u2 + q, dim::incommensurable_exception);
    CHECK_THROWS_AS(q + u2, dim::incommensurable_exception);
    CHECK_THROWS_AS(u2 - q, dim::incommensurable_exception);
    CHECK_THROWS_AS(q - u2, dim::incommensurable_exception);
#else
    result = u2 + q;
    CHECK(result.is_bad());

    result = q + u2;
    CHECK(result.is_bad());

    result = u2 - q;
    CHECK(result.is_bad());

    result = q - u2;
    CHECK(result.is_bad());
#endif

    result = u1 + q;
    CHECK(result.unit() == q.unit());
    CHECK(result.value() == doctest::Approx(6.0));

    result = q + u1;
    CHECK(result.unit() == q.unit());
    CHECK(result.value() == doctest::Approx(6.0));
        
    result = u1 - q;
    CHECK(result.unit() == q.unit());
    CHECK(result.value() == doctest::Approx(-4.0));

    result = q - u1;
    CHECK(result.unit() == q.unit());
    CHECK(result.value() == doctest::Approx(4.0));

    result = q;
    result += u1;
    CHECK(result.unit() == q.unit());
    CHECK(result.value() == doctest::Approx(6.0));

    result = q;
    result -= u1;
    CHECK(result.unit() == q.unit());
    CHECK(result.value() == doctest::Approx(4.0));
}

TEST_CASE("dynamic_quanity.unit_scalar")
{    
    si::dynamic_unit u1(si::Speed::unit{});

    si::dynamic_quantity q = 2.0 * u1;    
    CHECK(q.value() == 2.0);
    CHECK(q.unit() == u1);
    q = u1 * 2.0;
    CHECK(q.value() == 2.0);
    CHECK(q.unit() == u1);

    q = 2.0 / u1;
    CHECK(q.value() == 2.0);
    CHECK(q.unit() == inverse(u1));
    q = u1 / 2.0;
    CHECK(q.value() == 0.5);
    CHECK(q.unit() == u1);
}