#pragma once
#include "tag.hpp"
#include <algorithm>
#include <cstdint>

namespace dim
{

// Forward declare the dynamic_unit/index type
template <class System> class dynamic_unit;


/// Models the base units in a system (i.e. quantities where the magnitude is 1)
template <DIM_ARRAY, class System> struct unit : public unit_tag {
    using system = System;
    using inverse = unit<-Length, -Time, -Mass, -Angle, -Temperature, -Amount, -Current, -Luminosity, system>;
    using type = unit<DIM_D_ARRAY, System>;

    static constexpr int8_t length() { return Length; }
    static constexpr int8_t time() { return Time; }
    static constexpr int8_t mass() { return Mass; }
    static constexpr int8_t angle() { return Angle; }
    static constexpr int8_t temperature() { return Temperature; }
    static constexpr int8_t amount() { return Amount; }
    static constexpr int8_t current() { return Current; }
    static constexpr int8_t luminosity() { return Luminosity; }
};

/**
* @brief dynamic_units are a compact representation of the dimension string plus the system tag.
*
* These are comparable types and can be used as keys in maps.
*/
template <class System> class dynamic_unit : public dynamic_unit_tag
{
  private:
    enum dimension_order { DIM_D_ARRAY };

    /**
     * Get byte v as a byte of an 8-byte int, respecting its sign.
     */ 
    static constexpr uint64_t scast(int8_t v) 
    {
        // We must cast to an unsigned byte before bit manipulation, otherwise negative
        // number will be spread across the whole 64 bits (2's complement)
        return static_cast<uint64_t>(static_cast<uint8_t>(v));
    }

    /**
     * Put byte v at the i'th byte of the 8 byte int.
     */
    static constexpr uint64_t scast(int8_t v, uint8_t i) 
    {
        return scast(v) << (i*8);
    }

  public:
    using system = System;

    explicit constexpr dynamic_unit(uint64_t code)
        : m_code(code)
    {
    }

    // clang-format off
    constexpr dynamic_unit(int8_t i_length, int8_t i_time, int8_t i_mass, int8_t i_angle, int8_t i_temperature,
                           int8_t i_amount, int8_t i_current, int8_t i_luminosity)
        : m_code(scast(i_length, Length)           | scast(i_time, Time)     | scast(i_mass, Mass)       | scast(i_angle, Angle) 
               | scast(i_temperature, Temperature) | scast(i_amount, Amount) | scast(i_current, Current) | scast(i_luminosity, Luminosity))
    {
    }
    // clang-format on

    /**
     * @brief Transform a static unit to a dynamic_unit
     */
    template <class U, DIM_IS_UNIT(U)>
    explicit constexpr dynamic_unit(U const&) : dynamic_unit(from<U>()) { }

    /**
     * @brief Transform a static unit to a dynamic_unit
     */
    template <class U, DIM_IS_UNIT(U)> static constexpr dynamic_unit from()
    {
        return dynamic_unit(U::length(), U::time(), U::mass(), U::angle(), U::temperature(), U::amount(), U::current(),
                            U::luminosity());
    }

    /**
     * @brief Get the underlying uint64_t data for serialization.
     */
    constexpr uint64_t raw() const { return m_code; }

    // Access each dimension
    constexpr int8_t length() const { return get_byte(Length); }
    constexpr int8_t time() const { return get_byte(Time); }
    constexpr int8_t mass() const { return get_byte(Mass); }
    constexpr int8_t angle() const { return get_byte(Angle); }
    constexpr int8_t temperature() const { return get_byte(Temperature); }
    constexpr int8_t amount() const { return get_byte(Amount); }
    constexpr int8_t current() const { return get_byte(Current); }
    constexpr int8_t luminosity() const { return get_byte(Luminosity); }

    // Set each dimension
    void length(int8_t b) { return set_byte(Length, b); }
    void time(int8_t b) { return set_byte(Time, b); }
    void mass(int8_t b) { return set_byte(Mass, b); }
    void angle(int8_t b) { return set_byte(Angle, b); }
    void temperature(int8_t b) { return set_byte(Temperature, b); }
    void amount(int8_t b) { return set_byte(Amount, b); }
    void current(int8_t b) { return set_byte(Current, b); }
    void luminosity(int8_t b) { return set_byte(Luminosity, b); }

    // Comparison operators
    constexpr bool operator==(dynamic_unit rhs) const { return raw() == rhs.raw(); }

    constexpr bool operator<(dynamic_unit rhs) const { return raw() < rhs.raw(); }

    constexpr bool operator!=(dynamic_unit rhs) const { return !(*this == rhs); }

    constexpr bool operator>(dynamic_unit rhs) const { return rhs < *this; }

    constexpr bool operator<=(dynamic_unit rhs) const { return !(rhs < *this); }

    constexpr bool operator>=(dynamic_unit rhs) const { return !(*this < rhs); }

    constexpr static dynamic_unit dimensionless() { return dynamic_unit(0L); }

    constexpr static dynamic_unit bad_unit() { return dynamic_unit(~0L); }

    constexpr bool is_bad() const { return *this == bad_unit(); }

    /**
     * @brief Compute the product of two units.
     */
    // clang-format off
    constexpr dynamic_unit multiply(dynamic_unit const& i_other) const
    {        
        // Work byte by byte to avoid overflow into neighboring dimensions
        return dynamic_unit{
            scast(get_byte(Length) + i_other.get_byte(Length), Length)
            | scast(get_byte(Time) + i_other.get_byte(Time), Time)
            | scast(get_byte(Mass) + i_other.get_byte(Mass), Mass)
            | scast(get_byte(Angle) + i_other.get_byte(Angle), Angle)
            | scast(get_byte(Temperature) + i_other.get_byte(Temperature), Temperature)
            | scast(get_byte(Amount) + i_other.get_byte(Amount), Amount)
            | scast(get_byte(Current) + i_other.get_byte(Current), Current)
            | scast(get_byte(Luminosity) + i_other.get_byte(Luminosity), Luminosity)
        };        
    }
    // clang-format on

    /**
     * @brief  Number of distinct dimensions.
     */
    static constexpr int size() { return 8; }

    /**
     * @brief Get a dimension by index (0 <= i < 8). The order matches the enum
     * values in base_dimension.
     */
    constexpr int8_t get(uint8_t i) const { return get_byte(std::min<uint8_t>(7, i)); }

  private:

    // Unchecked version of get()
    constexpr int8_t get_byte(uint8_t i) const { return static_cast<int8_t>((m_code & (0xfful << (i * 8))) >> (i * 8)); }

    // Set a byte in the code. This creates a mask that zeros out byte i, then or's b into that place.
    void set_byte(uint8_t i, int8_t b)
    {
        uint64_t mask = ~(0xfful << (i*8));
        m_code = (m_code & mask) | scast(b, i);
    }

    uint64_t m_code;
};

/**
 * Conversion methods
 */
template <class U, DIM_IS_UNIT(U)>
constexpr dynamic_unit<typename U::system> index()
{
    using unit_type = dynamic_unit<typename U::system>;
    return unit_type::template from<U>();
}

template <class U, DIM_IS_UNIT(U)>
constexpr dynamic_unit<typename U::system> index(U const&)
{
    using unit_type = dynamic_unit<typename U::system>;
    return unit_type::template from<U>();
}

template <class System, DIM_IS_SYSTEM(System)> constexpr dynamic_unit<System> index(dynamic_unit<System> const& u)
{
    return u;
}


/**
 * @brief Struct template to perform roots (square root, etc)
 */
template <class Unit1, int root> struct unit_root {
    using type = ::dim::unit<Unit1::length() / root, Unit1::time() / root, Unit1::mass() / root, Unit1::angle() / root,
                             Unit1::temperature() / root, Unit1::amount() / root, Unit1::current() / root,
                             Unit1::luminosity() / root, typename Unit1::system>;
};

/// Convenience typedef for roots
template <class Unit1, int root> using unit_root_t = typename unit_root<Unit1, root>::type;

/// Struct template to perform powers on units
template <class Unit1, int pow> struct unit_pow {
    using type = ::dim::unit<Unit1::length() * pow, Unit1::time() * pow, Unit1::mass() * pow, Unit1::angle() * pow,
                             Unit1::temperature() * pow, Unit1::amount() * pow, Unit1::current() * pow,
                             Unit1::luminosity() * pow, typename Unit1::system>;
};

/// Convenience typedef for powers
template <class Unit1, int root> using unit_pow_t = typename unit_pow<Unit1, root>::type;

/// Struct template to perform unit division
template <class Unit1, class Unit2> struct unit_divide {
    using type =
        ::dim::unit<Unit1::length() - Unit2::length(), Unit1::time() - Unit2::time(), Unit1::mass() - Unit2::mass(),
                    Unit1::angle() - Unit2::angle(), Unit1::temperature() - Unit2::temperature(),
                    Unit1::amount() - Unit2::amount(), Unit1::current() - Unit2::current(),
                    Unit1::luminosity() - Unit2::luminosity(), typename Unit1::system>;
};

/// Convenience typedef for division
template <class Unit1, class Unit2> using unit_divide_t = typename unit_divide<Unit1, Unit2>::type;

/// Struct template to perform unit multiplication
template <class Unit1, class Unit2> struct unit_multiply {
    using type =
        ::dim::unit<Unit1::length() + Unit2::length(), Unit1::time() + Unit2::time(), Unit1::mass() + Unit2::mass(),
                    Unit1::angle() + Unit2::angle(), Unit1::temperature() + Unit2::temperature(),
                    Unit1::amount() + Unit2::amount(), Unit1::current() + Unit2::current(),
                    Unit1::luminosity() + Unit2::luminosity(), typename Unit1::system>;
};

/// Convenience typedef for multiplication
template <class Unit1, class Unit2> using unit_multiply_t = typename unit_multiply<Unit1, Unit2>::type;

//////////////////////////////////////////////////////////////////////////////
// For C++11, these can't be constexpr void function templates
/// Check that U1 and U2 have the same dimensions
#define DIM_CHECK_DIMENSIONS(U1, U2)                                                                                   \
    static_assert(U1::length() == U2::length(), "Length dimensions do not match.");                                    \
    static_assert(U1::time() == U2::time(), "Time dimensions do not match.");                                          \
    static_assert(U1::mass() == U2::mass(), "Mass dimensions do not match.");                                          \
    static_assert(U1::angle() == U2::angle(), "Angle dimensions do not match.");                                       \
    static_assert(U1::temperature() == U2::temperature(), "Temperature dimensions do not match.");                     \
    static_assert(U1::amount() == U2::amount(), "Amount (mole) dimensions do not match.");                             \
    static_assert(U1::current() == U2::current(), "Current dimensions do not match.");                                 \
    static_assert(U1::luminosity() == U2::luminosity(), "Luminosity dimensions do not match.");

/// Check that U1 and U2 use the same system
#define DIM_CHECK_SYSTEMS(U1, U2)                                                                                      \
    static_assert(std::is_same<typename U1::system, typename U2::system>::value,                                       \
                  "Systems of units do not match. Explicit conversion is required.");
///////////////////////////////////////////////////////////////////////////////

/*
 * dimensionless unit specializations
 */
template <class Other, class System>
constexpr Other operator*(Other const& value, typename System::dimensionless_unit const&)
{
    return value;
}
template <class Other, class System>
constexpr Other operator*(typename System::dimensionless_unit const&, Other const& value)
{
    return value;
}
template <class Other, class System>
constexpr Other operator/(Other const& value, typename System::dimensionless_unit const&)
{
    return value;
}
template <class Other, class System>
constexpr Other operator/(typename System::dimensionless_unit const&, Other const& value)
{
    return static_cast<Other>(1.0) / value;
}

/*
 * Unit on unit operators
 */
/// Unit/unit multiplication operator
template <class U1, class U2, DIM_IS_UNIT(U1), DIM_IS_UNIT(U2)>
constexpr unit_multiply_t<U1, U2> operator*(U1 const&, U2 const&)
{
    DIM_CHECK_SYSTEMS(U1, U2);
    ;
    return unit_multiply_t<U1, U2>();
}

/// unit/unit division operator
template <class U1, class U2, DIM_IS_UNIT(U1), DIM_IS_UNIT(U2)>
constexpr unit_divide_t<U1, U2> operator/(U1 const&, U2 const&)
{
    DIM_CHECK_SYSTEMS(U1, U2);
    return unit_divide_t<U1, U2>();
}

/// unit/unit power function
template <class U, int P, DIM_IS_UNIT(U)> constexpr unit_pow_t<U, P> pow(U const&) { return unit_pow_t<U, P>(); }

/// Compute the inverse of a dynamic unit
template <class System> inline constexpr dynamic_unit<System> inverse(dynamic_unit<System> const& u)
{
    // clang-format off
    return dynamic_unit<System>({
        static_cast<int8_t>(-u.length()),
        static_cast<int8_t>(-u.time()),
        static_cast<int8_t>(-u.mass()),
        static_cast<int8_t>(-u.angle()),
        static_cast<int8_t>(-u.temperature()),
        static_cast<int8_t>(-u.amount()),
        static_cast<int8_t>(-u.current()),
        static_cast<int8_t>(-u.luminosity())}
    );
    // clang-format on
}

template <class System, DIM_IS_SYSTEM(System)> constexpr dynamic_unit<System> pow(dynamic_unit<System> i_unit, int n)
{
    return dynamic_unit<System>{
        static_cast<int8_t>(n * i_unit.length()),
        static_cast<int8_t>(n * i_unit.time()),
        static_cast<int8_t>(n * i_unit.mass()),
        static_cast<int8_t>(n * i_unit.angle()),
        static_cast<int8_t>(n * i_unit.temperature()),
        static_cast<int8_t>(n * i_unit.amount()),
        static_cast<int8_t>(n * i_unit.current()),
        static_cast<int8_t>(n * i_unit.luminosity())
    };
}

} // namespace dim