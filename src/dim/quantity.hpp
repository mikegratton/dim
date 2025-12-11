#pragma once
#include <cmath>
#include <limits>
#include "dim/tag.hpp"
#include "unit.hpp"

/**
 * The static quantity template type.
 */

/* clang-format off */
namespace dim {

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
    union {double d; uint64_t i;} c{val};    
    return (c.i & BAD) == BAD;
}
#else
constexpr inline bool isbad__(double val)
{
    constexpr uint64_t BAD = 0x7ff0000000000000u;
    union {double d; uint64_t i;} c{val};    
    return (c.i & BAD) == BAD;
}
#endif
#else
constexpr inline bool isbad__(double val) { return std::isnan(val); }
#endif


/// Combination of a scalar and a unit. Most dim types are typedefs for template instatiations of this 
template<class Unit, class Scalar>
class quantity : public quantity_tag {

protected: 
    Scalar m_value;

public:
    using scalar = Scalar;    
    using unit = Unit;
    using system = typename Unit::system;
    using type = quantity<unit, scalar>;
    using dimensionless = typename unit::system::dimensionless_unit;

    constexpr quantity() noexcept { }
    constexpr explicit quantity(Scalar s) noexcept : m_value(s) { }
    constexpr quantity(unit const&) noexcept : m_value(1) { }
    
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    constexpr quantity(Q2 const& q) noexcept : m_value(dimensionless_cast(q))
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
    }
    
    template<class U2, DIM_IS_UNIT(U2)>
    constexpr quantity(U2 const&) noexcept : m_value(1) 
    { 
        DIM_CHECK_DIMENSIONS(unit, U2)
        DIM_CHECK_SYSTEMS(unit, U2)
    }

    /// Obtain a bad quantity with these units.
    static constexpr type bad_quantity() noexcept { return type(bad_double__()); }

    /// Detect if this is a bad quantity
    constexpr bool is_bad() const { return isbad__(m_value); }
    
    template<class U2, DIM_IS_UNIT(U2)>
    type& operator=(U2 const&) noexcept 
    { 
        DIM_CHECK_DIMENSIONS(unit, U2)
        DIM_CHECK_SYSTEMS(unit, U2)
        m_value = Scalar(1); return *this;         
    }
    
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    type& operator=(Q2 const& rhs) noexcept 
    { 
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        m_value = dimensionless_cast(rhs);
        return *this;         
    }
    
    constexpr type operator-() const noexcept { return type(-m_value); }

    constexpr operator Scalar() const
    {
        DIM_CHECK_DIMENSIONS(unit, dimensionless)       
        return m_value;
    }

    // = operators
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend type& operator+= (type& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        lhs.m_value += rhs.m_value;
        return lhs;
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend type& operator-= (type& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        lhs.m_value -= rhs.m_value;
        return lhs;
    }
    template<class U, DIM_IS_UNIT(U)>
    friend type& operator+= (type& lhs, U const&)
    {
        DIM_CHECK_DIMENSIONS(unit, U)
        DIM_CHECK_SYSTEMS(unit, U)
        lhs.m_value += static_cast<Scalar>(1.0);
        return lhs;
    }
    template<class U, DIM_IS_UNIT(U)>
    friend type& operator-= (type& lhs, U const&)
    {
        DIM_CHECK_DIMENSIONS(unit, U)
        DIM_CHECK_SYSTEMS(unit, U)
        lhs.m_value -= static_cast<Scalar>(1.0);
        return lhs;
    }
    template<class Scalar2, DIM_IS_SCALAR(Scalar2)>
    friend type& operator*= (type& lhs, Scalar2 const& rhs)
    {
        lhs.m_value *= rhs;
        return lhs;
    }
    template<class Scalar2, DIM_IS_SCALAR(Scalar2)>
    friend type& operator/= (type& lhs, Scalar2 const& rhs)
    {
        lhs.m_value /= rhs;
        return lhs;
    }

    // Comparison operators
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator== (type const& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)      
        return lhs.m_value == rhs.m_value;
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator< (type const& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        return lhs.m_value < rhs.m_value;
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator!= (type const& lhs, Q2 const& rhs)
    {
        return !(lhs == rhs);
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator> (type const& lhs, Q2 const& rhs)
    {
        return rhs < lhs;
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator<= (type const& lhs, Q2 const& rhs)
    {
        return !(lhs > rhs);
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator>= (type const& lhs, Q2 const& rhs)
    {
        return !(lhs < rhs);
    }

    Scalar v() const { return m_value; }

    /// Cast away the units to access the raw scalar
    friend constexpr Scalar dimensionless_cast(type const& r) { return r.m_value; }

    /// Cast away the units to access the raw scalar
    friend constexpr Scalar& dimensionless_cast(type& r) { return r.m_value; }
    
    // Quantity-on-quantity operators
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr quantity<unit_multiply_t<unit, typename Q2::unit>, scalar>
    operator* (type const& q1, Q2 const& q2)
    {    
        DIM_CHECK_SYSTEMS(unit, Q2::unit);     
        return quantity<unit_multiply_t<unit, typename Q2::unit>, scalar>
               (dimensionless_cast(q1) * dimensionless_cast(q2));
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr quantity<unit_divide_t<unit, typename Q2::unit>, scalar>
    operator/ (type const& q1, Q2 const& q2)
    {
        DIM_CHECK_SYSTEMS(unit, Q2::unit);       
        return quantity<unit_divide_t<unit, typename Q2::unit>, scalar>
               (q1.m_value / dimensionless_cast(q2));
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr type operator+ (type const& q1, Q2 const& q2)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        return type(dimensionless_cast(q1) + dimensionless_cast(q2));
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr type operator- (type const& q1, Q2 const& q2)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)       
        return type(dimensionless_cast(q1) - dimensionless_cast(q2));
    }

    // Mult/divide leading to a scalar
    friend constexpr scalar
    operator* (type const& q1, quantity<typename unit::inverse, scalar> const& q2)
    {
        return dimensionless_cast(q1) * dimensionless_cast(q2);
    }
    friend constexpr scalar
    operator/ (type const& q1, type const& q2)
    {
        return (dimensionless_cast(q1) / dimensionless_cast(q2));
    }

    // Quantity/scalar ops (requires scalar matches this template's scalar)
    friend constexpr type
    operator* (type const& q, scalar const& s)
    {
        return type(q.m_value * s);
    }
    friend constexpr type
    operator* (scalar const& s, type const& q)
    {
        return type(q.m_value * s);
    }
    friend constexpr type
    operator/ (type const& q, scalar const& s)
    {
        return type(q.m_value / s);
    }
    friend constexpr quantity<typename unit::inverse, scalar>
    operator/ (scalar const& s, type const& q)
    {
        return quantity<typename unit::inverse, scalar>(s / q.m_value);
    }

    // Quantity/unit ops
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_multiply_t<unit, U>, scalar>
    operator* (type const& q, U const&)
    {
        DIM_CHECK_SYSTEMS(unit, U) 
        return quantity<unit_multiply_t<unit, U>, scalar>(q.m_value);
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_multiply_t<unit, U>, scalar>
    operator* (U const& u, type const& q)
    {
        return q * u;
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_divide_t<unit, U>, scalar>
    operator/ (type const& q, U const&)
    {
        DIM_CHECK_SYSTEMS(unit, U) 
        return quantity<unit_divide_t<unit, U>, scalar> (q.m_value);
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_divide_t<U, unit>, scalar>
    operator/ (U const&, type const& q)
    {
        DIM_CHECK_SYSTEMS(unit, U) 
        return quantity<unit_divide_t<U, unit>, scalar> (q.m_value);
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr type
    operator-(type const& q, U const&)
    {
        DIM_CHECK_DIMENSIONS(U, unit)
        return type{q.m_value - Scalar(1)};
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr type
    operator-(U const&, type const& q)
    {
        DIM_CHECK_DIMENSIONS(U, unit)
        return type{Scalar(1) - q.m_value};
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr type
    operator+(type const& q, U const&)
    {
        DIM_CHECK_DIMENSIONS(U, unit)
        return type{q.m_value + Scalar(1)};
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr type
    operator+(U const&, type const& q)
    {
        DIM_CHECK_DIMENSIONS(U, unit)
        return type{Scalar(1) + q.m_value};
    }

    // Quantity-unit ops leading to a scalar result
    friend constexpr scalar operator/ (type const& q, unit const&)
    {
        return q.m_value;
    }
    friend constexpr scalar
    operator* (type const& q, typename unit::inverse const&)
    {
        return q.m_value;
    }
};

/**
 * Transform Q into a comparable type
 */
template <class Q, DIM_IS_QUANTITY(Q)>
constexpr dynamic_unit<typename Q::system> index(Q const&)
{
    return index<typename Q::unit>();
}

/**
 * Transform Q into a comparable type
 */
template <class Q, DIM_IS_QUANTITY(Q)>
constexpr dynamic_unit<typename Q::system> index()
{
    return index<typename Q::unit>();
}

/*
 * Scalar on unit -> quantity
 */
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<U, S> operator* (S const& value, U const&)
{
    return quantity<U, S> (value);
}
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<U, S> operator* (U const& unit, S const& value)
{
    return value * unit;
}
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<typename U::inverse, S> operator/ (S const& value, U const&)
{
    return quantity<typename U::inverse, S>(value);
}
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<U, S> operator/ (U const& unit, S const& value)
{
    return (static_cast<S>(1.0) / value) * unit;
}

} // namespace dim

// Math operations
namespace dim
{
/// Absolute value of the scalar part
template<class Q, DIM_IS_QUANTITY(Q)>
constexpr Q abs(Q const& q)
{
    return Q(std::abs(dimensionless_cast(q)));
}

/// Take the root of the scalar and the units
template<int Root, class Q, DIM_IS_QUANTITY(Q)>
constexpr quantity<typename unit_root<typename Q::unit, Root>::type, typename Q::scalar>
root(Q const& q)
{
    static_assert(Q::unit::length() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::time() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::mass() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::angle() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::temperature() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::amount() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::current() % Root == 0, "Dimension not divisble by root");
    static_assert(Q::unit::luminosity() % Root == 0, "Dimension not divisble by root");
    return quantity<unit_root_t<typename Q::unit, Root>, typename Q::scalar>
           (std::pow(dimensionless_cast(q), static_cast<typename Q::scalar>(1) / static_cast<typename Q::scalar>(Root)));
}

/// Take the squareroot of the scalar and the units
template<class Q, DIM_IS_QUANTITY(Q)>
constexpr quantity<typename unit_root<typename Q::unit, 2>::type, typename Q::scalar>
sqrt(Q const& q)
{
    static_assert(Q::unit::length() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::time() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::mass() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::angle() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::temperature() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::amount() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::current() % 2 == 0, "Dimension not divisble by root");
    static_assert(Q::unit::luminosity() % 2 == 0, "Dimension not divisble by root");
    return quantity<unit_root_t<typename Q::unit, 2>, typename Q::scalar> (std::sqrt(dimensionless_cast(q)));
}

/// Exponentiate the quantity
template<int Exponent, class Q>
constexpr
std::enable_if_t<std::is_base_of<quantity_tag, Q>::value,
    quantity<unit_pow_t<typename Q::unit, Exponent>, typename Q::scalar>>
    pow(Q const& q)
{
    return quantity<unit_pow_t<typename Q::unit, Exponent>, typename Q::scalar>
           (std::pow(dimensionless_cast(q), Exponent));
}


template<int Exponent, class U, DIM_IS_UNIT(U)>
constexpr
std::enable_if_t<std::is_base_of<unit_tag, U>::value, unit_pow_t<U, Exponent>>
        pow(U const&)
{
    return unit_pow_t<U, Exponent>();
}


/**
 * @brief Rational powers of a quantity
 */
template<int Num, int Denom, class Q, DIM_IS_QUANTITY(Q)>
constexpr quantity<unit_pow_t<unit_root_t<typename Q::unit, Denom>, Num>, typename Q::scalar>
ratpow(Q const& q)
{
    static_assert(Q::unit::length() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::time() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::mass() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::angle() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::temperature() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::amount() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::current() * Num % Denom == 0, "Dimension not divisble by root");
    static_assert(Q::unit::luminosity() * Num % Denom == 0, "Dimension not divisble by root");
    return quantity<unit_pow_t<unit_root_t<typename Q::unit, Denom>, Num>, typename Q::scalar>
           (std::pow(dimensionless_cast(q), static_cast<typename Q::scalar>(Num) / static_cast<typename Q::scalar>(Denom)));
}

}

/* clang-format on*/