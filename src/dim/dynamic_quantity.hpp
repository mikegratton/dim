#pragma once
#include <array>
#include <cstdint>

#include "base.hpp"
#include "system_creation_helper.hpp"

namespace dim {
/// Tag type for dynamic_quantity
struct dynamic_quantity_tag {};

using dynamic_unit = std::array<int8_t, 8>;

template <class U, DIM_IS_UNIT(U)>
constexpr dynamic_unit toDynamic()
{
    return {U::length(),      U::time(),   U::mass(),    U::angle(),
            U::temperature(), U::amount(), U::current(), U::luminosity()};
}

/// Obtains a unique index for a dynamic_quantity based on its dimension
inline int64_t index(dynamic_unit const& u)
{
    int64_t value = u[0];
    value += static_cast<int64_t>(u[1]) << 8;
    value += static_cast<int64_t>(u[2]) << 16;
    value += static_cast<int64_t>(u[3]) << 24;
    value += static_cast<int64_t>(u[4]) << 32;
    value += static_cast<int64_t>(u[5]) << 40;
    value += static_cast<int64_t>(u[6]) << 48;
    value += static_cast<int64_t>(u[7]) << 56;
    return value;
    // return *reinterpret_cast<int64_t const*>(&u);
}

/// Obtain the unique index for a compile-type unit U
template <class U, DIM_IS_UNIT(U)>
int64_t index()
{
    return index(toDynamic<U>());
}

/// Compute the inverse of a dynamic unit
inline constexpr dynamic_unit inverse(dynamic_unit const& u)
{
    return dynamic_unit{(int8_t)-u[0], (int8_t)-u[1], (int8_t)-u[2], (int8_t)-u[3],
                        (int8_t)-u[4], (int8_t)-u[5], (int8_t)-u[6], (int8_t)-u[7]};
}

/// Check that two dynamic unit types match
inline bool unitsMatch(dynamic_unit const& i_u1, dynamic_unit const& i_u2)
{
    return index(i_u1) == index(i_u2);
}

/// Run-time dimensioned quantities. These are inefficient compared to quantity<>, but
/// are convenient for input operations
template <class S, class System, DIM_IS_SCALAR(S)>
struct dynamic_quantity : public dynamic_quantity_tag {
    using scalar = S;
    using system = System;
    using type = dynamic_quantity<S, System>;
    scalar value;
    dynamic_unit unit;

    dynamic_quantity() = default;
    constexpr explicit dynamic_quantity(scalar i_v) : value(i_v), unit{0, 0, 0, 0, 0, 0, 0, 0} {}
    constexpr explicit dynamic_quantity(scalar i_v, dynamic_unit const& i_u) : value(i_v), unit(i_u) {}
    template <class Q, DIM_IS_QUANTITY(Q)>
    constexpr dynamic_quantity(Q const& i_q) : value(i_q.value), unit{toDynamic<typename Q::unit>()}
    {
    }

    template <class Q, DIM_IS_QUANTITY(Q)>
    constexpr Q as() const
    {
        if (unitsMatch(unit, toDynamic<typename Q::unit>())) { return Q(value); }
        return Q::bad_quantity();
    }

    static constexpr dynamic_quantity<S, System> bad_quantity() { return dynamic_quantity<S, System>(bad_double__()); }
    constexpr bool is_bad() const { return isbad__(value); }

    constexpr int8_t length() const { return unit[base_dimension::Length]; }
    constexpr int8_t time() const { return unit[base_dimension::Time]; }
    constexpr int8_t mass() const { return unit[base_dimension::Mass]; }
    constexpr int8_t angle() const { return unit[base_dimension::Angle]; }
    constexpr int8_t temperature() const { return unit[base_dimension::Temperature]; }
    constexpr int8_t amount() const { return unit[base_dimension::Amount]; }
    constexpr int8_t current() const { return unit[base_dimension::Current]; }
    constexpr int8_t luminosity() const { return unit[base_dimension::Luminosity]; }

    constexpr bool dimensionless() const { return (index(unit) == 0L); }

    scalar const& dimensionless_cast(type const& q) { return q.value; }
    scalar& dimensionless_cast(type& q) { return q.value; }

    template <class S2>
    dynamic_quantity<S, System>& operator=(dynamic_quantity<S2, System> const& rhs)
    {
        value = rhs.value;
        unit = rhs.unit;
        return *this;
    }

    template <class S2>
    friend dynamic_quantity<S, System> multiply(dynamic_quantity<S, System> const& a,
                                                dynamic_quantity<S2, System> const& b)
    {
        dynamic_quantity<S, System> c;
        c.value = a.value * b.value;
        for (unsigned int i = 0; i < a.unit.size(); i++) { c.unit[i] = a.unit[i] + b.unit[i]; }
        return c;
    }

    template <class S2>
    friend dynamic_quantity<S, System> divide(dynamic_quantity<S, System> const& a,
                                              dynamic_quantity<S2, System> const& b)
    {
        dynamic_quantity<S, System> c;
        c.value = a.value / b.value;
        for (unsigned int i = 0; i < a.unit.size(); i++) { c.unit[i] = a.unit[i] - b.unit[i]; }
        return c;
    }

    template <class S2>
    friend dynamic_quantity<S, System> add(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit, b.unit)) { return dynamic_quantity<S, System>(a.value + b.value, a.unit); }
        return dynamic_quantity<S, System>::bad_quantity();
    }

    template <class S2>
    friend dynamic_quantity<S, System> subtract(dynamic_quantity<S, System> const& a,
                                                dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit, b.unit)) { return dynamic_quantity<S, System>(a.value - b.value, a.unit); }
        return dynamic_quantity<S, System>::bad_quantity();
    }

    template <class S2>
    friend dynamic_quantity<S, System> operator+(dynamic_quantity<S, System> const& a,
                                                 dynamic_quantity<S2, System> const& b)
    {
        return add(a, b);
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator-(dynamic_quantity<S, System> const& a,
                                                 dynamic_quantity<S2, System> const& b)
    {
        return subtract(a, b);
    }

    friend dynamic_quantity<S, System> operator-(dynamic_quantity<S, System> const& a)
    {
        return dynamic_quantity<S, System>(-a.value, a.unit);
    }

    template <class S2>
    friend dynamic_quantity<S, System> operator*(dynamic_quantity<S, System> const& a,
                                                 dynamic_quantity<S2, System> const& b)
    {
        return multiply(a, b);
    }

    template <class S2>
    friend dynamic_quantity<S, System> operator/(dynamic_quantity<S, System> const& a,
                                                 dynamic_quantity<S2, System> const& b)
    {
        return divide(a, b);
    }

    template <class S2>
    friend dynamic_quantity<S, System>& operator+=(dynamic_quantity<S, System>& a,
                                                   dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit, b.unit)) {
            a.value += b.value;
            return a;
        }
        a = a.bad();
        return a;
    }

    template <class S2>
    friend dynamic_quantity<S, System>& operator-=(dynamic_quantity<S, System>& a,
                                                   dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit, b.unit)) {
            a.value -= b.value;
            return a;
        }
        a = a.bad();
        return a;
    }

    template <class S2>
    friend dynamic_quantity<S, System>& operator*=(dynamic_quantity<S, System>& a,
                                                   dynamic_quantity<S2, System> const& b)
    {
        return (a = multiply(a, b));
    }

    template <class S2>
    friend dynamic_quantity<S, System>& operator/=(dynamic_quantity<S, System>& a,
                                                   dynamic_quantity<S2, System> const& b)
    {
        return (a = divide(a, b));
    }

    template <class S2>
    friend bool operator<(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit, b.unit)) { return a.value < b.value; }
        return false;
    }

    template <class S2>
    friend bool operator==(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit, b.unit)) { return a.value == b.value; }
        return false;
    }

    template <class S2>
    friend bool operator>(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        return b < a;
    }

    template <class S2>
    friend bool operator<=(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        return !(b < a);
    }

    template <class S2>
    friend bool operator>=(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        return !(a < b);
    }

    template <class S2>
    friend dynamic_quantity<S, System> operator*(S2 a, dynamic_quantity<S, System> const& b)
    {
        return dynamic_quantity<S, System>(b.value * a, b.unit);
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator/(S2 a, dynamic_quantity<S, System> const& b)
    {
        return dynamic_quantity<S, System>(a / b.value, inverse(b.unit));
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator*(dynamic_quantity<S, System> const& a, S2 b)
    {
        return dynamic_quantity<S, System>(a.value * b, a.unit);
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator/(dynamic_quantity<S, System> const& a, S2 b)
    {
        return dynamic_quantity<S, System>(a.value / b, a.unit);
    }
    template <class S2>
    friend dynamic_quantity<S, System>& operator*=(dynamic_quantity<S, System>& a, S2 b)
    {
        a.value *= b;
        return a;
    }
    template <class S2>
    friend dynamic_quantity<S, System>& operator/=(dynamic_quantity<S, System>& a, S2 b)
    {
        a.value /= b;
        return a;
    }
};

/// Take a power of a dynamic quantity
template <class S2, class System, DIM_IS_SCALAR(S2)>
dynamic_quantity<S2, System> power(dynamic_quantity<S2, System> const& a, int n)
{
    dynamic_quantity<S2, System> c;
    c.value = std::pow(a.value, n);
    for (unsigned int i = 0; i < a.unit.size(); i++) { c.unit[i] = a.unit[i] * n; }
    return c;
}

}  // namespace dim
