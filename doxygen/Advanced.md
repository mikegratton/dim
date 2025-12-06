
# Advanced Topics

## Metaprogramming with Quantities
One pitfall for C++ class templates is that each instantiation is a totally independent class at 
run time with no relation to any other instantiation.  Thus `si::Length` and `si::Time` are no 
more related than `std::string` and `double`.  This is often painful for code that wants to handle 
these types in a uniform way.  To aid with this, Dim provides a macro `DIM_IS_QUANTITY` that works 
with the C++ Substitution Failure is not an Error (SFINAE -- this language has the *worst* jargon).  
It works like this
```cpp
template<class Q, DIM_IS_QUANTITY(Q)>
char* print_quantity(char* o_quant_str, Q const& q)
{
    int offset = sprintf(o_quant_str, "%g_", static_cast<double>(q.value));
    print_unit(o_quant_str+offset, q);
    return o_quant_str;
}
```
The print_quantity function template is thus only defined for quantity types, where we know `q` 
will have a member `value`.


## Micro-Optimization for Nondimensionalization

If you need to nondimensionalize a quantity say for serialization, you may want to squeeze out 
all the performance you can while still using the facilities of Dim to catch errors.  For example,
here are three ways to nondimensionalize a length:
```cpp
Length L = 2_m;
double v1 = dimensionless_cast(L); // 1
double v1 = L / meter; // 2
double v2 = L / meter_; // 3
```
(1) is an unchecked access and can be unsafe. (2) does just what it says,
provide the length of L in meters. However, since `meter` is itself a Length
quantity, this actually performs a floating point division (2.0/1.0).  (3) uses
the specially defined *unit* types (these are the same as the quantities but
have a trailing underscore).  The `operator/` action here understands that a
unit has, well, *unit* value, so avoids the division.  In fact, at runtime, (1)
and (3) produce the same machine instructions.

## Fractional Dimensions

Dim does not support fractional dimension like "m^1/2" that are used in some domains.  Supporting
these really complicates the metaprogramming and makes your program compile slower.  You can typically
work with e.g. squared quantities to obtain integer dimensions. (If your application has fractal 
dimensions, why are you using this library at all?) If this is too much of a burden, the 
[boost::units](https://www.boost.org/doc/libs/1_80_0/doc/html/boost_units.html)
library provides this feature.


## Bad Quantities and NaN 

When `__FAST_MATH__` is defined, Dim uses an expanded notion of a "bad" floating point number that 
includes numbers that are Inf, -Inf, and NaN in IEEE 754 floating point. The rationale is that on 
embedded systems (ARM in particular), you often must compile with `-Ofast` to obtain access to SIMD 
instructions. When you do this, `std::isnan()` always evaluates to false.  Dim uses its own 
`is_bad()` function to work around this. The trade-off is that algorithms relying on correct 
behavior with Inf will see values as "bad". But such algorithms are usually limited to specialized 
scientific codes rather than the more general robotics and engineering codes Dim is intended for.

Note if `__FAST_MATH__` isn't defined, then `is_bad()` is an alias for `isnan()`.
