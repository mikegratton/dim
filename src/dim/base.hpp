#pragma once
#include <cmath>
#include <concepts>
#include <cstdint>
#include <limits>
#include <type_traits>

#if __cplusplus < 202002L
#error "C++20 or higher is required for dim"
#endif

/// The dim namespace encloses all dim symbols
namespace dim {

/*
 * Tag types for concepts
 */

/// Tag type for units
struct unit_tag {};

/// Tag type for quantities
struct quantity_tag {};

/// Tag type for systems
struct system_tag {};

template <typename T>
concept UnitType = std::is_base_of<unit_tag, T>::value;

template <typename T>
concept QuantityType = std::is_base_of<quantity_tag, T>::value;

template <typename T>
concept SystemType = std::is_base_of<system_tag, T>::value;

template <typename T>
concept ScalarType = std::is_arithmetic<T>::value;

// Dimension list
#define DIM_ARRAY                                                                                             \
    int8_t Length, int8_t Time, int8_t Mass, int8_t Angle, int8_t Temperature, int8_t Amount, int8_t Current, \
        int8_t Luminosity

/*
 * The unit class has no data -- its properties are all in template parameters.
 * Access to these is provided by member functions. The System class is a tag
 * to facilitate different unit systems side by side.
 */
template <DIM_ARRAY, class System>
struct unit;

/*
 * A quantity marries a scalar value with a unit
 */
template <class Unit, class Scalar = double>
class quantity;

/********************************************************************************************/
/// Models the base units in a system (i.e. quantities where the magnitude is 1)
template <DIM_ARRAY, class System>
struct unit : public unit_tag {
    using system = System;
    using inverse = unit<-Length, -Time, -Mass, -Angle, -Temperature, -Amount, -Current, -Luminosity, system>;
    using type = unit<Length, Time, Mass, Angle, Temperature, Amount, Current, Luminosity, System>;

    static constexpr int length() { return Length; }
    static constexpr int time() { return Time; }
    static constexpr int mass() { return Mass; }
    static constexpr int angle() { return Angle; }
    static constexpr int temperature() { return Temperature; }
    static constexpr int amount() { return Amount; }
    static constexpr int current() { return Current; }
    static constexpr int luminosity() { return Luminosity; }

    /// Obtain a numeric index that represents the unit type.
    /// NOTE this does not distinguish between systems
    static constexpr uint64_t index()
    {
        return static_cast<uint64_t>((uint8_t)Length) | static_cast<uint64_t>((uint8_t)Time) << 8 |
               static_cast<uint64_t>((uint8_t)Mass) << 16 | static_cast<uint64_t>((uint8_t)Angle) << 24 |
               static_cast<uint64_t>((uint8_t)Temperature) << 32 | static_cast<uint64_t>((uint8_t)Amount) << 40 |
               static_cast<uint64_t>((uint8_t)Current) << 48 | static_cast<uint64_t>((uint8_t)Luminosity) << 56;
    }
};

/**
 * @brief Struct template to perform roots (squareroot, etc)
 */
template <class Unit1, int root>
struct unit_root {
    using type = ::dim::unit<Unit1::length() / root, Unit1::time() / root, Unit1::mass() / root, Unit1::angle() / root,
                             Unit1::temperature() / root, Unit1::amount() / root, Unit1::current() / root,
                             Unit1::luminosity() / root, typename Unit1::system>;
};

/// Convenience typedef for roots
template <class Unit1, int root>
using unit_root_t = typename unit_root<Unit1, root>::type;

/// Struct template to perform powers on units
template <class Unit1, int pow>
struct unit_pow {
    using type = ::dim::unit<Unit1::length() * pow, Unit1::time() * pow, Unit1::mass() * pow, Unit1::angle() * pow,
                             Unit1::temperature() * pow, Unit1::amount() * pow, Unit1::current() * pow,
                             Unit1::luminosity() * pow, typename Unit1::system>;
};

/// Convenience typedef for powers
template <class Unit1, int root>
using unit_pow_t = typename unit_pow<Unit1, root>::type;

/// Struct template to perform unit division
template <class Unit1, class Unit2>
struct unit_divide {
    using type =
        ::dim::unit<Unit1::length() - Unit2::length(), Unit1::time() - Unit2::time(), Unit1::mass() - Unit2::mass(),
                    Unit1::angle() - Unit2::angle(), Unit1::temperature() - Unit2::temperature(),
                    Unit1::amount() - Unit2::amount(), Unit1::current() - Unit2::current(),
                    Unit1::luminosity() - Unit2::luminosity(), typename Unit1::system>;
};

/// Convenience typedef for division
template <class Unit1, class Unit2>
using unit_divide_t = typename unit_divide<Unit1, Unit2>::type;

/// Struct template to perform unit multiplication
template <class Unit1, class Unit2>
struct unit_multiply {
    using type =
        ::dim::unit<Unit1::length() + Unit2::length(), Unit1::time() + Unit2::time(), Unit1::mass() + Unit2::mass(),
                    Unit1::angle() + Unit2::angle(), Unit1::temperature() + Unit2::temperature(),
                    Unit1::amount() + Unit2::amount(), Unit1::current() + Unit2::current(),
                    Unit1::luminosity() + Unit2::luminosity(), typename Unit1::system>;
};

/// Convenience typedef for multiplication
template <class Unit1, class Unit2>
using unit_multiply_t = typename unit_multiply<Unit1, Unit2>::type;

//////////////////////////////////////////////////////////////////////////////
/// Check that U1 and U2 have the same dimensions
template <UnitType U1, UnitType U2>
constexpr void ensure_dimensions_match()
{
    static_assert(U1::length() == U2::length(), "Length dimensions do not match.");
    static_assert(U1::time() == U2::time(), "Time dimensions do not match.");
    static_assert(U1::mass() == U2::mass(), "Mass dimensions do not match.");
    static_assert(U1::angle() == U2::angle(), "Angle dimensions do not match.");
    static_assert(U1::temperature() == U2::temperature(), "Temperature dimensions do not match.");
    static_assert(U1::amount() == U2::amount(), "Amount (mole) dimensions do not match.");
    static_assert(U1::current() == U2::current(), "Current dimensions do not match.");
    static_assert(U1::luminosity() == U2::luminosity(), "Luminosity dimensions do not match.");
}

/// Check that U1 and U2 use the same system
template <UnitType U1, UnitType U2>
constexpr void ensure_systems_match()
{
    static_assert(std::is_same<typename U1::system, typename U2::system>::value,
                  "Systems of units do not match. Explicit conversion is required.");
}

///////////////////////////////////////////////////////////////////////////////

/*
 * dimensionless unit specializations
 */
template <class Other, class System>
constexpr Other operator*(Other const& value, typename System::dimensionless_unit const& unit)
{
    return value;
}
template <class Other, class System>
constexpr Other operator*(typename System::dimensionless_unit const& unit, Other const& value)
{
    return value;
}
template <class Other, class System>
constexpr Other operator/(Other const& value, typename System::dimensionless_unit const& unit)
{
    return value;
}
template <class Other, class System>
constexpr Other operator/(typename System::dimensionless_unit const& unit, Other const& value)
{
    return static_cast<Other>(1.0) / value;
}

/*
 * Unit on unit operators
 */
/// Unit/unit multiplication operator
template <UnitType U1, UnitType U2>
constexpr unit_multiply_t<U1, U2> operator*(U1 const&, U2 const&)
{
    ensure_systems_match<U1, U2>;
    return unit_multiply_t<U1, U2>();
}

/// unit/unit division operator
template <UnitType U1, UnitType U2>
constexpr unit_divide_t<U1, U2> operator/(U1 const&, U2 const&)
{
    ensure_systems_match<U1, U2>;
    return unit_divide_t<U1, U2>();
}

/// unit/unit power function
template <UnitType U, int P>
constexpr unit_pow_t<U, P> pow(U const&)
{
    return unit_pow_t<U, P>();
}

/// Obtain a quiet nan even in fastmath mode
constexpr inline double bad_double__()
{
    return std::numeric_limits<double>::quiet_NaN();
}

/**
 * @brief Special isnan to operate when in non-IEEE compliant mode
 */
#ifdef __FAST_MATH__
#if __cplusplus < 201402L
inline bool isbad__(double val)
{
    constexpr uint64_t BAD = 0x7ff0000000000000u;
    union {
        double d;
        uint64_t i;
    } c{val};
    return (c.i & BAD) == BAD;
}
#else
constexpr inline bool isbad__(double val)
{
    constexpr uint64_t BAD = 0x7ff0000000000000u;
    union {
        double d;
        uint64_t i;
    } c{val};
    return (c.i & BAD) == BAD;
}
#endif
#else
constexpr inline bool isbad__(double val)
{
    return std::isnan(val);
}
#endif

/// Combination of a scalar and a unit. Most dim types are typedefs for template instatiations of this
template <class Unit, class Scalar>
class quantity : public quantity_tag {
   protected:
    Scalar value;

   public:
    using scalar = Scalar;
    using unit = Unit;
    using type = quantity<unit, scalar>;
    using dimensionless = typename unit::system::dimensionless_unit;

    constexpr quantity() noexcept {}
    constexpr explicit quantity(Scalar s) noexcept : value(s) {}
    constexpr quantity(unit const&) noexcept : value(static_cast<scalar>(1.0)) {}

    template <QuantityType Q2>
    constexpr quantity(Q2 const& q) noexcept : value(dimensionless_cast(q))
    {
        ensure_dimensions_match<unit, Q2::unit>();
        ensure_systems_match<unit, Q2::unit>;
    }

    template <UnitType U2>
    constexpr quantity(U2 const&) noexcept : value(static_cast<scalar>(1.0))
    {
        ensure_dimensions_match<unit, U2>();
        ensure_systems_match<unit, U2>();
    }

    /// Obtain a numeric index that represents the unit type.
    /// NOTE this does not distinguish between systems
    static constexpr uint64_t index() { return unit::index(); }

    /// Obtain a bad quantity with these units.
    static constexpr type bad_quantity() noexcept { return type(bad_double__()); }

    /// Detect if this is a bad quantity
    constexpr bool is_bad() const { return isbad__(value); }

    template <UnitType U2>
    type& operator=(U2 const&) noexcept
    {
        ensure_dimensions_match<unit, U2>();
        ensure_systems_match<unit, U2>();
        value = static_cast<scalar>(1);
        return *this;
    }

    template <QuantityType Q2>
    type& operator=(Q2 const& rhs) noexcept
    {
        ensure_dimensions_match<unit, Q2::unit>();
        ensure_systems_match<unit, Q2::unit>();
        value = dimensionless_cast(rhs);
        return *this;
    }

    constexpr type operator-() const noexcept { return type(-value); }

    constexpr operator Scalar() const
    {
        ensure_dimensions_match<unit, dimensionless>();
        return value;
    }

    // = operators
    template <QuantityType Q2>
    friend type& operator+=(type& lhs, Q2 const& rhs)
    {
        ensure_dimensions_match<unit, Q2::unit>();
        ensure_systems_match<unit, Q2::unit>();
        lhs.value += rhs.value;
        return lhs;
    }
    template <QuantityType Q2>
    friend type& operator-=(type& lhs, Q2 const& rhs)
    {
        ensure_dimensions_match<unit, Q2::unit>();
        ensure_systems_match<unit, Q2::unit>();
        lhs.value -= rhs.value;
        return lhs;
    }
    template <UnitType U>
    friend type& operator+=(type& lhs, U const&)
    {
        ensure_dimensions_match<unit, U>();
        ensure_systems_match<unit, U>();
        lhs.value += static_cast<Scalar>(1.0);
        return lhs;
    }
    template <UnitType U>
    friend type& operator-=(type& lhs, U const&)
    {
        ensure_dimensions_match<unit, U>();
        ensure_systems_match<unit, U>();
        lhs.value -= static_cast<Scalar>(1.0);
        return lhs;
    }
    template <ScalarType Scalar2>
    friend type& operator*=(type& lhs, Scalar2 const& rhs)
    {
        lhs.value *= rhs;
        return lhs;
    }
    template <ScalarType Scalar2>
    friend type& operator/=(type& lhs, Scalar2 const& rhs)
    {
        lhs.value /= rhs;
        return lhs;
    }

    // Comparison operators
    template <QuantityType Q2>
    friend constexpr bool operator==(type const& lhs, Q2 const& rhs)
    {
        ensure_dimensions_match<unit, typename Q2::unit>();
        ensure_systems_match<unit, typename Q2::unit>();
        return lhs.value == rhs.value;
    }
    template <QuantityType Q2>
    friend constexpr bool operator<(type const& lhs, Q2 const& rhs)
    {
        ensure_dimensions_match<unit, typename Q2::unit>();
        ensure_systems_match<unit, typename Q2::unit>();
        return lhs.value < rhs.value;
    }
    template <QuantityType Q2>
    friend constexpr bool operator!=(type const& lhs, Q2 const& rhs)
    {
        return !(lhs == rhs);
    }
    template <QuantityType Q2>
    friend constexpr bool operator>(type const& lhs, Q2 const& rhs)
    {
        return rhs < lhs;
    }
    template <QuantityType Q2>
    friend constexpr bool operator<=(type const& lhs, Q2 const& rhs)
    {
        return !(lhs > rhs);
    }
    template <QuantityType Q2>
    friend constexpr bool operator>=(type const& lhs, Q2 const& rhs)
    {
        return !(lhs < rhs);
    }

    Scalar v() const { return value; }

    /// Cast away the units to access the raw scalar
    friend constexpr Scalar dimensionless_cast(type const& r) { return r.value; }

    /// Cast away the units to access the raw scalar
    friend constexpr Scalar& dimensionless_cast(type& r) { return r.value; }

    // Quantity-on-quantity operators
    template <QuantityType Q2>
    friend constexpr quantity<unit_multiply_t<unit, typename Q2::unit>, scalar> operator*(type const& q1, Q2 const& q2)
    {
        ensure_systems_match<unit, typename Q2::unit>();
        return quantity<unit_multiply_t<unit, typename Q2::unit>, scalar>(dimensionless_cast(q1) *
                                                                          dimensionless_cast(q2));
    }
    template <QuantityType Q2>
    friend constexpr quantity<unit_divide_t<unit, typename Q2::unit>, scalar> operator/(type const& q1, Q2 const& q2)
    {
        ensure_systems_match<unit, typename Q2::unit>();
        return quantity<unit_divide_t<unit, typename Q2::unit>, scalar>(q1.value / dimensionless_cast(q2));
    }
    template <QuantityType Q2>
    friend constexpr type operator+(type const& q1, Q2 const& q2)
    {
        ensure_dimensions_match<unit, typename Q2::unit>();
        ensure_systems_match<unit, typename Q2::unit>();
        return type(dimensionless_cast(q1) + dimensionless_cast(q2));
    }
    template <QuantityType Q2>
    friend constexpr type operator-(type const& q1, Q2 const& q2)
    {
        ensure_dimensions_match<unit, typename Q2::unit>();
        ensure_systems_match<unit, typename Q2::unit>();
        return type(dimensionless_cast(q1) - dimensionless_cast(q2));
    }

    // Mult/divide leading to a scalar
    friend constexpr scalar operator*(type const& q1, quantity<typename unit::inverse, scalar> const& q2)
    {
        return dimensionless_cast(q1) * dimensionless_cast(q2);
    }
    friend constexpr scalar operator/(type const& q1, type const& q2)
    {
        return (dimensionless_cast(q1) / dimensionless_cast(q2));
    }

    // Quantity/scalar ops (requires scalar matches this template's scalar)
    friend constexpr type operator*(type const& q, scalar const& s) { return type(q.value * s); }
    friend constexpr type operator*(scalar const& s, type const& q) { return type(q.value * s); }
    friend constexpr type operator/(type const& q, scalar const& s) { return type(q.value / s); }
    friend constexpr quantity<typename unit::inverse, scalar> operator/(scalar const& s, type const& q)
    {
        return quantity<typename unit::inverse, scalar>(s / q.value);
    }

    // Quantity/unit ops
    template <UnitType U>
    friend constexpr quantity<unit_multiply_t<unit, U>, scalar> operator*(type const& q, U const&)
    {
        ensure_systems_match<unit, U>();
        return quantity<unit_multiply_t<unit, U>, scalar>(q.value);
    }
    template <UnitType U>
    friend constexpr quantity<unit_multiply_t<unit, U>, scalar> operator*(U const& u, type const& q)
    {
        return q * u;
    }
    template <UnitType U>
    friend constexpr quantity<unit_divide_t<unit, U>, scalar> operator/(type const& q, U const&)
    {
        ensure_systems_match<unit, U>();
        return quantity<unit_divide_t<unit, U>, scalar>(q.value);
    }
    template <UnitType U>
    friend constexpr quantity<unit_divide_t<U, unit>, scalar> operator/(U const&, type const& q)
    {
        ensure_systems_match<unit, U>();
        return quantity<unit_divide_t<U, unit>, scalar>(q.value);
    }

    // Quantity-unit ops leading to a scalar result
    friend constexpr scalar operator/(type const& q, unit const&) { return q.value; }
    friend constexpr scalar operator*(type const& q, typename unit::inverse const&) { return q.value; }
};

/*
 * Scalar on unit -> quantity
 */
template <UnitType U, ScalarType S = double>
constexpr quantity<U, S> operator*(S const& value, U const&)
{
    return quantity<U, S>(value);
}
template <UnitType U, ScalarType S = double>
constexpr quantity<U, S> operator*(U const& unit, S const& value)
{
    return value * unit;
}
template <UnitType U, ScalarType S = double>
constexpr quantity<typename U::inverse, S> operator/(S const& value, U const&)
{
    return quantity<typename U::inverse, S>(value);
}
template <UnitType U, ScalarType S = double>
constexpr quantity<U, S> operator/(U const& unit, S const& value)
{
    return (static_cast<S>(1.0) / value) * unit;
}

}  // namespace dim

// Math operations
namespace dim {
/// Absolute value of the scalar part
template <QuantityType Q>
constexpr Q abs(Q const& q)
{
    return Q(std::abs(dimensionless_cast(q)));
}

/// Take the root of the scalar and the units
template <int Root, QuantityType Q>
constexpr quantity<typename unit_root<typename Q::unit, Root>::type, typename Q::scalar> root(Q const& q)
{
    static_assert(Q::unit::length() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::time() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::mass() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::angle() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::temperature() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::amount() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::current() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::luminosity() % Root == 0, "Dimension not divisble by root");
    return quantity<unit_root_t<typename Q::unit, Root>, typename Q::scalar>(
        std::pow(dimensionless_cast(q), static_cast<typename Q::scalar>(1) / static_cast<typename Q::scalar>(Root)));
}

/// Take the squareroot of the scalar and the units
template <QuantityType Q>
constexpr quantity<typename unit_root<typename Q::unit, 2>::type, typename Q::scalar> sqrt(Q const& q)
{
    static_assert(Q::unit::length() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::time() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::mass() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::angle() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::temperature() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::amount() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::current() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::luminosity() % 2 == 0, "Dimension not divisble by root");
    return quantity<unit_root_t<typename Q::unit, 2>, typename Q::scalar>(std::sqrt(dimensionless_cast(q)));
}

/// Exponentiate the quantity
template <int Exponent, QuantityType Q>
constexpr quantity<unit_pow_t<typename Q::unit, Exponent>, typename Q::scalar> pow(Q const& q)
{
    return quantity<unit_pow_t<typename Q::unit, Exponent>, typename Q::scalar>(
        std::pow(dimensionless_cast(q), Exponent));
}

template <int Exponent, UnitType U>
constexpr unit_pow_t<U, Exponent> pow(U const&)
{
    return unit_pow_t<U, Exponent>();
}

/**
 * @brief Rational powers of a quantity
 */
template <int Num, int Denom, QuantityType Q>
constexpr quantity<unit_pow_t<unit_root_t<typename Q::unit, Denom>, Num>, typename Q::scalar> ratpow(Q const& q)
{
    static_assert(Q::unit::length() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::time() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::mass() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::angle() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::temperature() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::amount() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::current() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::luminosity() * Num % Denom == 0, "Dimension not divisble by root");
    return quantity<unit_pow_t<unit_root_t<typename Q::unit, Denom>, Num>, typename Q::scalar>(
        std::pow(dimensionless_cast(q), static_cast<typename Q::scalar>(Num) / static_cast<typename Q::scalar>(Denom)));
}

}  // namespace dim
