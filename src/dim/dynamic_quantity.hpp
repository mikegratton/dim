#pragma once
#include <array>
#include <cstdint>

#include "base.hpp"
#include "system_creation_helper.hpp"

namespace dim {
/// Tag type for dynamic_quantity
struct dynamic_quantity_tag {};

using dynamic_unit = std::array<int8_t, 8>;

template <UnitType U>
constexpr dynamic_unit toDynamic()
{
    return {U::length(),      U::time(),   U::mass(),    U::angle(),
            U::temperature(), U::amount(), U::current(), U::luminosity()};
}

/// Obtains a unique index for a dynamic_quantity based on its dimension
inline uint64_t index(dynamic_unit const& u)
{
    uint64_t value = static_cast<uint64_t>((uint8_t)u[0]) | static_cast<uint64_t>((uint8_t)u[1]) << 8 |
                     static_cast<uint64_t>((uint8_t)u[2]) << 16 | static_cast<uint64_t>((uint8_t)u[3]) << 24 |
                     static_cast<uint64_t>((uint8_t)u[4]) << 32 | static_cast<uint64_t>((uint8_t)u[5]) << 40 |
                     static_cast<uint64_t>((uint8_t)u[6]) << 48 | static_cast<uint64_t>((uint8_t)u[7]) << 56;
    return value;
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
template <ScalarType S, class System>
struct dynamic_quantity : public dynamic_quantity_tag {
    using scalar = S;
    using system = System;
    using type = dynamic_quantity<S, System>;
    scalar value_;
    dynamic_unit unit_;

    dynamic_quantity() = default;
    constexpr explicit dynamic_quantity(scalar i_v) : value_(i_v), unit_{0, 0, 0, 0, 0, 0, 0, 0} {}
    constexpr explicit dynamic_quantity(scalar i_v, dynamic_unit const& i_u) : value_(i_v), unit_(i_u) {}
    template <QuantityType Q>
    constexpr dynamic_quantity(Q const& i_q) : value_(i_q.value), unit_{toDynamic<typename Q::unit>()}
    {
    }

    dynamic_unit const& unit() const { return unit_; }
    dynamic_unit& unit() { return unit_; }
    int8_t unit(std::size_t i) { return unit_[i]; }
    void unit(dynamic_unit const& u) { unit_ = u; }
    scalar value() const { return value_; }
    void value(scalar v) { value_ = v; }

    template <QuantityType Q>
    constexpr Q as() const
    {
        if (unitsMatch(unit(), toDynamic<typename Q::unit>())) { return Q(value()); }
        return Q::bad_quantity();
    }

    static constexpr dynamic_quantity<S, System> bad_quantity() { return dynamic_quantity<S, System>(bad_double__()); }
    constexpr bool is_bad() const { return isbad__(value_); }

    constexpr int8_t length() const { return unit_[base_dimension::Length]; }
    constexpr int8_t time() const { return unit_[base_dimension::Time]; }
    constexpr int8_t mass() const { return unit_[base_dimension::Mass]; }
    constexpr int8_t angle() const { return unit_[base_dimension::Angle]; }
    constexpr int8_t temperature() const { return unit_[base_dimension::Temperature]; }
    constexpr int8_t amount() const { return unit_[base_dimension::Amount]; }
    constexpr int8_t current() const { return unit_[base_dimension::Current]; }
    constexpr int8_t luminosity() const { return unit_[base_dimension::Luminosity]; }

    void length(int8_t i) { unit_[base_dimension::Length] = i; }
    void time(int8_t i) { unit_[base_dimension::Time] = i; }
    void mass(int8_t i) { unit_[base_dimension::Mass] = i; }
    void angle(int8_t i) { unit_[base_dimension::Angle] = i; }
    void temperature(int8_t i) { unit_[base_dimension::Temperature] = i; }
    void amount(int8_t i) { unit_[base_dimension::Amount] = i; }
    void current(int8_t i) { unit_[base_dimension::Current] = i; }
    void luminosity(int8_t i) { unit_[base_dimension::Luminosity] = i; }

    constexpr bool dimensionless() const { return (index(unit_) == 0L); }

    scalar const& dimensionless_cast(type const& q) { return q.value; }
    scalar& dimensionless_cast(type& q) { return q.value; }
    uint64_t index() const { return ::dim::index(unit_); }

    template <class S2>
    dynamic_quantity<S, System>& operator=(dynamic_quantity<S2, System> const& rhs)
    {
        value_ = rhs.value_;
        unit_ = rhs.unit_;
        return *this;
    }

    template <class S2>
    friend dynamic_quantity<S, System> multiply(dynamic_quantity<S, System> const& a,
                                                dynamic_quantity<S2, System> const& b)
    {
        dynamic_quantity<S, System> c;
        c.value_ = a.value_ * b.value_;
        for (unsigned int i = 0; i < a.unit_.size(); i++) { c.unit_[i] = a.unit_[i] + b.unit_[i]; }
        return c;
    }

    template <class S2>
    friend dynamic_quantity<S, System> divide(dynamic_quantity<S, System> const& a,
                                              dynamic_quantity<S2, System> const& b)
    {
        dynamic_quantity<S, System> c;
        c.value_ = a.value_ / b.value_;
        for (unsigned int i = 0; i < a.unit_.size(); i++) { c.unit_[i] = a.unit_[i] - b.unit_[i]; }
        return c;
    }

    template <class S2>
    friend dynamic_quantity<S, System> add(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit_, b.unit_)) { return dynamic_quantity<S, System>(a.value_ + b.value_, a.unit_); }
        return dynamic_quantity<S, System>::bad_quantity();
    }

    template <class S2>
    friend dynamic_quantity<S, System> subtract(dynamic_quantity<S, System> const& a,
                                                dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit_, b.unit_)) { return dynamic_quantity<S, System>(a.value_ - b.value_, a.unit_); }
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
        return dynamic_quantity<S, System>(-a.value_, a.unit_);
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
        if (unitsMatch(a.unit_, b.unit_)) {
            a.value_ += b.value_;
            return a;
        }
        a = a.bad();
        return a;
    }

    template <class S2>
    friend dynamic_quantity<S, System>& operator-=(dynamic_quantity<S, System>& a,
                                                   dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit_, b.unit_)) {
            a.value_ -= b.value_;
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
        if (unitsMatch(a.unit_, b.unit_)) { return a.value_ < b.value_; }
        return false;
    }

    template <class S2>
    friend bool operator==(dynamic_quantity<S, System> const& a, dynamic_quantity<S2, System> const& b)
    {
        if (unitsMatch(a.unit_, b.unit_)) { return a.value_ == b.value_; }
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
        return dynamic_quantity<S, System>(b.value_ * a, b.unit_);
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator/(S2 a, dynamic_quantity<S, System> const& b)
    {
        return dynamic_quantity<S, System>(a / b.value_, inverse(b.unit_));
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator*(dynamic_quantity<S, System> const& a, S2 b)
    {
        return dynamic_quantity<S, System>(a.value_ * b, a.unit_);
    }
    template <class S2>
    friend dynamic_quantity<S, System> operator/(dynamic_quantity<S, System> const& a, S2 b)
    {
        return dynamic_quantity<S, System>(a.value_ / b, a.unit_);
    }
    template <class S2>
    friend dynamic_quantity<S, System>& operator*=(dynamic_quantity<S, System>& a, S2 b)
    {
        a.value_ *= b;
        return a;
    }
    template <class S2>
    friend dynamic_quantity<S, System>& operator/=(dynamic_quantity<S, System>& a, S2 b)
    {
        a.value_ /= b;
        return a;
    }
};

/// Take a power of a dynamic quantity
template <ScalarType S2, class System>
dynamic_quantity<S2, System> power(dynamic_quantity<S2, System> const& a, int n)
{
    dynamic_quantity<S2, System> c;
    c.value_ = std::pow(a.value_, n);
    for (unsigned int i = 0; i < a.unit_.size(); i++) { c.unit_[i] = a.unit_[i] * n; }
    return c;
}

}  // namespace dim
