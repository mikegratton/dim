#pragma once
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

/* clang-format off */

#if __cplusplus < 201103L
#error "C++11 or higher is required for dim"
#endif

#if __cplusplus < 201402L
namespace std
{
template< bool B, class T = void >
using enable_if_t = typename ::std::enable_if<B,T>::type;    
}
#endif

/// The dim namespace encloses all dim symbols
namespace dim
{

/*
 * Tag types for dispatch/enable_if
 */

/// Tag type for units
struct unit_tag { };

/// Tag type for quantities
struct quantity_tag { };

/// Tag type for systems
struct system_tag { };


// SFINAE macros that can be used as template parameters

/// Use as a template parameter to check if TYPE has been tagged TAG
#define DIM_IS_TAGGED_FOR(TAG, TYPE) typename std::enable_if_t<std::is_base_of<TAG, TYPE>::value>* = nullptr

/// Use as a template parameter to check if U is a unit
#define DIM_IS_UNIT(U) DIM_IS_TAGGED_FOR(::dim::unit_tag, U)

/// Use as a template parameter to check if Q is a quantity
#define DIM_IS_QUANTITY(Q) DIM_IS_TAGGED_FOR(::dim::quantity_tag, Q)

/// Use as a template parameter to check if T is a system
#define DIM_IS_SYSTEM(T) DIM_IS_TAGGED_FOR(::dim::system_tag, T)

/// Check if S is a scalar type (float, double, etc)
#define DIM_IS_SCALAR(S) typename std::enable_if_t<std::is_arithmetic<S>::value>* = nullptr

// Macros for dimension list
#define DIM_ARRAY int Length, int Time, int Mass, int Angle, int Temperature, int Amount, int Current, int Luminosity
#define DIM_D_ARRAY Length, Time, Mass, Angle, Temperature, Amount, Current, Luminosity

/*
 * The unit class has no data -- its properties are all in template parameters.
 * Access to these is provided by member functions. The System class is a tag
 * to facilitate different unit systems side by side.
 */
template<DIM_ARRAY, class System>
struct unit;

/*
 * A quantity marries a scalar value with a unit
 */
template<class Unit, class Scalar = double>
class quantity;


/**
 * @brief Unifying base class template for units
 */
template<class T>
struct unit_base : unit_tag {
    using type = T;
};

/********************************************************************************************/
/// Models the base units in a system (i.e. quantities where the magnitude is 1)
template<DIM_ARRAY, class System>
struct unit : public unit_base<unit<DIM_D_ARRAY, System>> {
    using system = System;
    using inverse = unit < -Length, -Time, -Mass, -Angle, -Temperature, -Amount, -Current, -Luminosity,
          system >;
    using type = unit<DIM_D_ARRAY, System>;

    static constexpr int length()
    {
        return Length;
    }
    static constexpr int time()
    {
        return Time;
    }
    static constexpr int mass()
    {
        return Mass;
    }
    static constexpr int angle()
    {
        return Angle;
    }
    static constexpr int temperature()
    {
        return Temperature;
    }
    static constexpr int amount()
    {
        return Amount;
    }
    static constexpr int current()
    {
        return Current;
    }
    static constexpr int luminosity()
    {
        return Luminosity;
    }
};

/// Obtain the inverse of U (i.e. if U is L, inverse_t<U> is 1/L)
template<class U, DIM_IS_UNIT(U)>
using inverse_t = typename U::inverse;

/**
 * @brief Struct template to perform roots (squareroot, etc)
 */
template<class Unit1, int root, DIM_IS_UNIT(Unit1)>
struct unit_root {
    using type = ::dim::unit <
                 Unit1::length()      / root,
                 Unit1::time()        / root,
                 Unit1::mass()        / root,
                 Unit1::angle()       / root,
                 Unit1::temperature() / root,
                 Unit1::amount()      / root,
                 Unit1::current()     / root,
                 Unit1::luminosity()  / root,
                 typename Unit1::system >;
};

/// Convenience typedef for roots
template<class Unit1, int root, DIM_IS_UNIT(Unit1)>
using unit_root_t = typename unit_root<Unit1, root>::type;

/// Struct template to perform powers on units
template<class Unit1, int pow, DIM_IS_UNIT(Unit1)>
struct unit_pow {
    using type = ::dim::unit <
                 Unit1::length()*       pow,
                 Unit1::time()*         pow,
                 Unit1::mass()*         pow,
                 Unit1::angle()*        pow,
                 Unit1::temperature()*  pow,
                 Unit1::amount()*       pow,
                 Unit1::current()*      pow,
                 Unit1::luminosity()*   pow,
                 typename Unit1::system >;
};

/// Convenience typedef for powers
template<class Unit1, int root, DIM_IS_UNIT(Unit1)>
using unit_pow_t = typename unit_pow<Unit1, root>::type;

/// Struct template to perform unit division
template<class Unit1, class Unit2, DIM_IS_UNIT(Unit1), DIM_IS_UNIT(Unit2)>
struct unit_divide {
    using type = ::dim::unit <
                 Unit1::length()      - Unit2::length(),
                 Unit1::time()        - Unit2::time(),
                 Unit1::mass()        - Unit2::mass(),
                 Unit1::angle()       - Unit2::angle(),
                 Unit1::temperature() - Unit2::temperature(),
                 Unit1::amount()      - Unit2::amount(),
                 Unit1::current()     - Unit2::current(),
                 Unit1::luminosity()  - Unit2::luminosity(),
                 typename Unit1::system >;
};

/// Convenience typedef for division
template<class Unit1, class Unit2>
using unit_divide_t = typename unit_divide<Unit1, Unit2>::type;

/// Struct template to perform unit multiplication
template<class Unit1, class Unit2, DIM_IS_UNIT(Unit1), DIM_IS_UNIT(Unit2)>
struct unit_multiply {
    using type = ::dim::unit <
                 Unit1::length()      + Unit2::length(),
                 Unit1::time()        + Unit2::time(),
                 Unit1::mass()        + Unit2::mass(),
                 Unit1::angle()       + Unit2::angle(),
                 Unit1::temperature() + Unit2::temperature(),
                 Unit1::amount()      + Unit2::amount(),
                 Unit1::current()     + Unit2::current(),
                 Unit1::luminosity()  + Unit2::luminosity(),
                 typename Unit1::system >;
};

/// Convenience typedef for multiplication
template<class Unit1, class Unit2>
using unit_multiply_t = typename unit_multiply<Unit1, Unit2>::type;

//////////////////////////////////////////////////////////////////////////////
// For C++11, these can't be constexpr void function templates
/// Check that U1 and U2 have the same dimensions
#define DIM_CHECK_DIMENSIONS(U1, U2) \
static_assert(U1::length() == U2::length(), "Length dimensions do not match.");\
static_assert(U1::time() == U2::time(), "Time dimensions do not match.");\
static_assert(U1::mass() == U2::mass(), "Mass dimensions do not match.");\
static_assert(U1::angle() == U2::angle(), "Angle dimensions do not match.");\
static_assert(U1::temperature() == U2::temperature(), "Temperature dimensions do not match.");\
static_assert(U1::amount() == U2::amount(), "Amount (mole) dimensions do not match.");\
static_assert(U1::current() == U2::current(), "Current dimensions do not match.");\
static_assert(U1::luminosity() == U2::luminosity(), "Luminosity dimensions do not match.");

/// Check that U1 and U2 use the same system
#define DIM_CHECK_SYSTEMS(U1, U2) \
static_assert(std::is_same<typename U1::system, typename U2::system>::value,\
                  "Systems of units do not match. Explicit conversion is required.");
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Conversion to the underlying scalar type is done by recognizing this type:
 */
template<class System>
using dimensionless_unit = unit<0, 0, 0, 0, 0, 0, 0, 0, System>;

/*
 * dimensionless unit specializations
 */
template<class Other, class System, DIM_IS_SYSTEM(System)>
constexpr Other operator* (Other const& value, dimensionless_unit<System> const& unit)
{
    return value;
}
template<class Other, class System, DIM_IS_SYSTEM(System)>
constexpr Other operator* (dimensionless_unit<System> const& unit, Other const& value)
{
    return value;
}
template<class Other, class System, DIM_IS_SYSTEM(System)>
constexpr Other operator/ (Other const& value, dimensionless_unit<System> const& unit)
{
    return value;
}
template<class Other, class System, DIM_IS_SYSTEM(System)>
constexpr Other operator/ (dimensionless_unit<System> const& unit, Other const& value)
{
    return static_cast<Other>(1.0) / value;
}

/*
 * Unit on unit operators
 */
 /// Unit/unit multiplication operator
template<class U1, class U2, DIM_IS_UNIT(U1), DIM_IS_UNIT(U2)>
constexpr unit_multiply_t<U1, U2>
operator* (unit_base<U1> const&, unit_base<U2> const&)
{
    DIM_CHECK_SYSTEMS(U1, U2);;
    return unit_multiply_t<U1, U2>();
}

/// unit/unit division operator
template<class U1, class U2, DIM_IS_UNIT(U1), DIM_IS_UNIT(U2)>
constexpr unit_divide_t<U1, U2>
operator/ (unit_base<U1> const&, unit_base<U2> const&)
{
    DIM_CHECK_SYSTEMS(U1, U2);
    return unit_divide_t<U1, U2>();
}

/// unit/unit power function
template<class U, int P, DIM_IS_UNIT(U)>
constexpr unit_pow_t<U, P>
pow(unit_base<U> const&)
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
    Scalar value;

public:
    using scalar = Scalar;
    using unit = Unit;
    using type = quantity<unit, scalar>;
    using dimensionless = dimensionless_unit<typename Unit::system>;

    constexpr quantity() noexcept { }
    constexpr explicit quantity(Scalar s) noexcept : value(s) { }
    constexpr quantity(unit const&) noexcept : value(static_cast<scalar>(1.0)) { }
    
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    constexpr quantity(Q2 const& q) noexcept : value(dimensionless_cast(q))
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
    }
    
    template<class U2, DIM_IS_UNIT(U2)>
    constexpr quantity(unit const&) noexcept : value(static_cast<scalar>(1.0)) 
    { 
        DIM_CHECK_DIMENSIONS(unit, U2)
        DIM_CHECK_SYSTEMS(unit, U2)
    }
    
    /// Obtain a bad quantity with these units.
    static constexpr type bad_quantity() noexcept { return type(bad_double__()); }

    /// Detect if this is a bad quantity
    constexpr bool is_bad() const { return isbad__(value); }
    
    template<class U2, DIM_IS_UNIT(U2)>
    type& operator=(U2 const&) noexcept 
    { 
        DIM_CHECK_DIMENSIONS(unit, U2)
        DIM_CHECK_SYSTEMS(unit, U2)
        value = static_cast<scalar>(1); return *this;         
    }
    
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    type& operator=(Q2 const& rhs) noexcept 
    { 
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        value = dimensionless_cast(rhs);
        return *this;         
    }
    
    constexpr type operator-() const noexcept { return type(-value); }

    constexpr operator Scalar() const
    {
        DIM_CHECK_DIMENSIONS(unit, dimensionless)       
        return value;
    }


    // = operators
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend type& operator+= (type& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        lhs.value += rhs.value;
        return lhs;
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend type& operator-= (type& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        lhs.value -= rhs.value;
        return lhs;
    }
    template<class U, DIM_IS_UNIT(U)>
    friend type& operator+= (type& lhs, unit_base<U> const&)
    {
        using U2 =  typename unit_base<U>::type;
        DIM_CHECK_DIMENSIONS(unit, U2)
        DIM_CHECK_SYSTEMS(unit, U2)
        lhs.value += static_cast<Scalar>(1.0);
        return lhs;
    }
    template<class U, DIM_IS_UNIT(U)>
    friend type& operator-= (type& lhs, unit_base<U> const&)
    {
        using U2 =  typename unit_base<U>::type;
        DIM_CHECK_DIMENSIONS(unit, U2)
        DIM_CHECK_SYSTEMS(unit, U2)
        lhs.value -= static_cast<Scalar>(1.0);
        return lhs;
    }
    template<class Scalar2, DIM_IS_SCALAR(Scalar2)>
    friend type& operator*= (type& lhs, Scalar2 const& rhs)
    {
        lhs.value *= rhs;
        return lhs;
    }
    template<class Scalar2, DIM_IS_SCALAR(Scalar2)>
    friend type& operator/= (type& lhs, Scalar2 const& rhs)
    {
        lhs.value /= rhs;
        return lhs;
    }

    // Comparison operators
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator== (type const& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)      
        return lhs.value == rhs.value;
    }
    template<class Q2, DIM_IS_QUANTITY(Q2)>
    friend constexpr bool operator< (type const& lhs, Q2 const& rhs)
    {
        DIM_CHECK_DIMENSIONS(unit, Q2::unit)
        DIM_CHECK_SYSTEMS(unit, Q2::unit)
        return lhs.value < rhs.value;
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

    /// Cast away the units to access the raw scalar
    friend constexpr Scalar const& dimensionless_cast(type const& r) { return r.value; }

    /// Cast away the units to access the raw scalar
    friend constexpr Scalar& dimensionless_cast(type& r) { return r.value; }
    
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
               (q1.value / dimensionless_cast(q2));
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
    operator* (type const& q1, quantity<inverse_t<unit>, scalar> const& q2)
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
        return type(q.value * s);
    }
    friend constexpr type
    operator* (scalar const& s, type const& q)
    {
        return type(q.value * s);
    }
    friend constexpr type
    operator/ (type const& q, scalar const& s)
    {
        return type(q.value / s);
    }
    friend constexpr quantity<inverse_t<unit>, scalar>
    operator/ (scalar const& s, type const& q)
    {
        return quantity<inverse_t<unit>, scalar> (s / q.value);
    }

    // Quantity/unit ops
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_multiply_t<unit, U>, scalar>
    operator* (type const& q, unit_base<U> const&)
    {
        DIM_CHECK_SYSTEMS(unit, U) 
        return quantity<unit_multiply_t<unit, U>, scalar>(q.value);
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_multiply_t<unit, U>, scalar>
    operator* (unit_base<U> const& u, type const& q)
    {
        return q * u;
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_divide_t<unit, U>, scalar>
    operator/ (type const& q, unit_base<U> const&)
    {
        DIM_CHECK_SYSTEMS(unit, U) 
        return quantity<unit_divide_t<unit, U>, scalar> (q.value);
    }
    template<class U, DIM_IS_UNIT(U)>
    friend constexpr quantity<unit_divide_t<U, unit>, scalar>
    operator/ (unit_base<U> const&, type const& q)
    {
        DIM_CHECK_SYSTEMS(unit, U) 
        return quantity<unit_divide_t<U, unit>, scalar> (q.value);
    }

    // Quantity-unit ops leading to a scalar result
    friend constexpr scalar operator/ (type const& q, unit const&)
    {
        return q.value;
    }
    friend constexpr scalar
    operator* (type const& q, inverse_t<unit> const&)
    {
        return q.value;
    }
};


/*
 * Scalar on unit -> quantity
 */
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<U, S> operator* (S const& value, unit_base<U> const&)
{
    return quantity<U, S> (value);
}
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<U, S> operator* (unit_base<U> const& unit, S const& value)
{
    return value * unit;
}
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<inverse_t<U>, S> operator/ (S const& value, unit_base<U> const&)
{
    return quantity<inverse_t<U>, S > (value);
}
template<class U, class S=double, DIM_IS_UNIT(U), DIM_IS_SCALAR(S) >
constexpr quantity<U, S> operator/ (unit_base<U> const& unit, S const& value)
{
    return (static_cast<S>(1.0) / value) * unit;
}

}

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

template<int Exponent, class U>
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