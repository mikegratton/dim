#pragma once
#include "quantity.hpp"
#include <limits>

#ifdef DIM_EXCEPTIONS
#include "incommensurable_exception.hpp"
#endif

namespace dim
{

/**
 * @brief Run-time dimensioned quantities. 
 *
 * These are inefficient compared to quantity<>, but are convenient for input
 * operations.
 *
 * @note While these function largely the same as the static types, operator= is
 * fundamentally different. Following C++ practice, this copies the object on
 * the right hand side. Thus
 * ```
 * si::dynamic_quantity myLength(5.0_m);
 * si::dynamic_quantity myTime(2.0_s);
 * myLength = myTime; // myLength now has units of time
 * ```
 *
 * @note If DIM_EXCEPTIONS is true, addition and quantity-casting operations may
 * through incommensurable_exception if the dimensions are not correct.
 */
template <class S, class System, DIM_IS_SCALAR(S)>
class dynamic_quantity : public dynamic_quantity_tag
{
  public:
    using scalar = S;
    using system = System;
    using type = dynamic_quantity<S, System>;
    using unit_type = dynamic_unit<System>;

    constexpr dynamic_quantity(scalar i_v, unit_type const& i_u)
        : m_value(i_v),
          m_unit(i_u)
    {
    }

    explicit constexpr dynamic_quantity(scalar i_v)
        : dynamic_quantity(i_v, unit_type::dimensionless())
    {
    }

    constexpr dynamic_quantity(unit_type const& i_unit)
        : dynamic_quantity(scalar(1), i_unit)
    {
    }

    template <class Q, DIM_IS_QUANTITY(Q)>
    constexpr dynamic_quantity(Q const& i_q)
        : dynamic_quantity(dimensionless_cast(i_q), index(i_q))
    {
    }

    constexpr dynamic_quantity()
        : dynamic_quantity(scalar(0), unit_type::dimensionless())
    {
    }

    template <class U, DIM_IS_UNIT(U)>
    constexpr dynamic_quantity(scalar i_v, U const& i_u)
        : dynamic_quantity(i_v, dim::index(i_u))
    {
    }

    constexpr unit_type const& unit() const { return m_unit; }
    void unit(unit_type const& i_u) { m_unit = i_u; }
    constexpr scalar value() const { return m_value; }
    void value(scalar i_v) { m_value = i_v; }

    constexpr operator scalar() const
    {
        #ifdef DIM_EXCEPTIONS
        if (unit() != unit_type::dimensionless()) {
            throw incommensurable_exception(unit(), unit_type::dimensionless(), "dynamic_quantity is not dimensionless");
        }
        #endif
        return value();
    }

#if __cplusplus >= 201402L
    template <class Q, DIM_IS_QUANTITY(Q)>
    constexpr
#else
    template <class Q, DIM_IS_QUANTITY(Q)>
#endif
        Q
        as() const
    {
        if (unit() == ::dim::index<typename Q::unit>()) {
            return Q(value());
        }
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(unit(), ::dim::index<typename Q::unit>(),
                                        "Could not convert dynamic_quantity to quantity");
#else
        return Q::bad_quantity();
#endif
    }

    static constexpr type bad_quantity()
    {
        return type(std::numeric_limits<scalar>::quiet_NaN(), unit_type::bad_unit());
    }
    constexpr bool is_bad() const { return isbad__(m_value); }

    constexpr bool dimensionless() const { return (m_unit == unit_type::dimensionless()); }

    template <class S2, DIM_IS_SCALAR(S2)>
    type& operator=(dynamic_quantity<S2, System> const& i_rhs)
    {
        m_value = static_cast<scalar>(i_rhs.m_value);
        m_unit = i_rhs.m_unit;
        return *this;
    }

    // Quantity/quantity opertors and functions
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type multiply(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return {a.m_value * b.m_value, a.m_unit.multiply(b.m_unit)};
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type divide(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return multiply(a, dynamic_quantity<S2, System>(S2(1) / b.m_value, inverse(b.unit())));
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend type add(type const& a, dynamic_quantity<S2, System> const& b)
    {
        if (a.unit() == b.unit()) {
            return {a.m_value + b.m_value, a.unit()};
        }
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(a.unit(), b.unit(), "Could not add quantities");
#else
        return type::bad_quantity();
#endif
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend type subtract(type const& a, dynamic_quantity<S2, System> const& b)
    {
        if (a.unit() == b.unit()) {
            return {a.m_value - b.m_value, a.unit()};
        }
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(a.unit(), b.unit(), "Could not subtract quantities");
#else
        return type::bad_quantity();
#endif
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator+(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return add(a, b);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator-(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return subtract(a, b);
    }

    friend type operator-(type const& a) { return type(-a.m_value, a.unit()); }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type operator*(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return multiply(a, b);
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type operator/(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return divide(a, b);
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend type& operator+=(type& a, dynamic_quantity<S2, System> const& b)
    {
        if (a.unit() == b.unit()) {
            a.m_value += b.m_value;
            return a;
        }
        a = bad_quantity();
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(a.unit(), b.unit(), "Could not add/accumulate quantities");
#else
        return a;
#endif
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend type& operator-=(type& a, dynamic_quantity<S2, System> const& b)
    {
        if (a.unit() == b.unit()) {
            a.m_value -= b.m_value;
            return a;
        }
        a = bad_quantity();
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(a.unit(), b.unit(), "Could not subtract/accumulate quantities");
#else
        return a;
#endif
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type& operator*=(type& a, dynamic_quantity<S2, System> const& b)
    {
        return (a = multiply(a, b));
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type& operator/=(type& a, dynamic_quantity<S2, System> const& b)
    {
        return (a = divide(a, b));
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend bool operator<(type const& a, dynamic_quantity<S2, System> const& b)
    {
        if (a.unit() == b.unit()) {
            return a.m_value < b.m_value;
        }
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(a.unit(), b.unit(), "Could not compare quantities");
#else
        return false;
#endif
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend bool operator==(type const& a, dynamic_quantity<S2, System> const& b)
    {
        if (a.unit() == b.unit()) {
            return a.m_value == b.m_value;
        }
#ifdef DIM_EXCEPTIONS
        throw incommensurable_exception(a.unit(), b.unit(), "Could not compare quantities");
#else
        return false;
#endif
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend bool operator!=(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return !(a == b);
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr bool operator>(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return b < a;
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr bool operator<=(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return !(b < a);
    }

    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr bool operator>=(type const& a, dynamic_quantity<S2, System> const& b)
    {
        return !(a < b);
    }

    // Scalar/quantity operators
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type operator*(S2 a, type const& b)
    {
        return type(b.m_value * a, b.m_unit);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type operator/(S2 a, type const& b)
    {
        return type(a / b.m_value, inverse(b.m_unit));
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type operator*(type const& a, S2 b)
    {
        return type(a.m_value * b, a.m_unit);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type operator/(type const& a, S2 b)
    {
        return type(a.m_value / b, a.m_unit);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator+(S2 a, type const& b)
    {
        return b + type(a);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator+(type const& b, S2 a)
    {
        return b + type(a);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator-(S2 a, type const& b)
    {
        return type(a) - b;
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator-(type const& b, S2 a)
    {
        return b - type(a);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type& operator*=(type& a, S2 b)
    {
        return a *= type(b);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend constexpr type& operator/=(type& a, S2 b)
    {
        return a /= type(b);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator+=(type& a, S2 b)
    {
        return a += type(a);
    }
    template <class S2, DIM_IS_SCALAR(S2)>
    friend type operator-=(type& a, S2 b)
    {
        return a -= type(a);
    }

    // Quantity/unit operators
    friend constexpr type operator*(unit_type const& i_unit, type const& i_quantity) { return type(i_unit) * i_quantity; }
    friend constexpr type operator*(type const& i_quantity, unit_type const& i_unit) { return type(i_unit) * i_quantity; }
    friend constexpr type operator/(unit_type const& i_unit, type const& i_quantity) { return type(i_unit) / i_quantity; }
    friend constexpr type operator/(type const& i_quantity, unit_type const& i_unit) { return i_quantity / type(i_unit); }
    friend type operator+(unit_type const& i_unit, type const& i_quantity) { return type(i_unit) + i_quantity; }
    friend type operator+(type const& i_quantity, unit_type const& i_unit) { return type(i_unit) + i_quantity; }
    friend type operator-(unit_type const& i_unit, type const& i_quantity) { return type(i_unit) - i_quantity; }
    friend type operator-(type const& i_quantity, unit_type const& i_unit) { return i_quantity - type(i_unit); }
    friend constexpr type& operator*=(type& a, unit_type const& b) { return a *= type(b); }
    friend constexpr type& operator/=(type& a, unit_type const& b) { return a /= type(b); }
    friend type& operator+=(type& a, unit_type const& b) { return a += type(b); }
    friend type& operator-=(type& a, unit_type const& b) { return a -= type(b); }

  private:
    scalar m_value;
    unit_type m_unit;
};

template <class Scalar, class System>
constexpr dynamic_unit<System> index(dynamic_quantity<Scalar, System> const& q)
{
    return q.unit();
}

template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
DQ::scalar dimensionless_cast(DQ const& q)
{
    return q.value();
}

/// Take a power of a dynamic quantity
template <class DQ, DIM_IS_DYNAMIC_QUANTITY(DQ)>
DQ power(DQ const& a, int n)
{
    return {std::pow(dimensionless_cast(a), n), pow(a.unit(), n)};
}


} // namespace dim
