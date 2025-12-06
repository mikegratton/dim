
# Basic Dimensional Analysis

In the standard low-energy physics model, there are seven dimensions:

1. Length
2. Time
3. Mass
4. Amount of substance
5. Luminous intensity
6. Temperature
7. Electrical current

To these Dim adds an eighth: angle, discussed below. For each of these eight 
dimensions, a quantity may have an integer multiplier. For example, acceleration has
dimensions of L/T^2 or one length, minus two time dimensions.

Dimensional analysis is best understood in analogy to vectors. Just as a vector can be broken down 
into a magnitude and a direction, a dimensional quantity is the product of a nondimensional scalar
and a dimensional unit.  The name is suggestive, and it should be: the direction for a vector
is itself a vector with unit magnitude. The "direction" of a dimensional quantity is itself a 
dimensional quantity with a unit scalar -- i.e., a "unit" dimension.  Dimensional quantities obey
a simple algebra: q1 and q2 can be summed if their units match. The product q1*q2 is
s1 * s2 * (u1+u2), where qi has scalar si and unit ui.  Here, unit addition obeys vector rules.


## NIST SP 811 Standard and the Angle Unit
[The National Institute of Standards and Technology Special Publication 811](https://www.nist.gov/pml/special-publication-811)
is the most complete guide to SI units. Wherever possible, Dim uses the NIST SP 811 unit names, abbreviations,
and so on. Note that in this standard, plurals are generally avoided, so we write "meter" and not "meters."

There is one major exception: Dim considers Angle to be a fundamental dimension. This choice resolves
a lot of ambiguities in the standard system.  For instance, the bedevilment caused by wondering if
a frequency is Hz or is a *rotational frequency* is resolved. These have different units: Hz is 1/s while
rotational frequency is rad/s. Other notable changes are that the units of torque ang luminous flux.
The units of torque are N m / rad, so that if you apply this torque through a full revolution, you see the 
work done is (N m / rad) * (2 * pi * rad) = 2 * pi * W. Likewise the  lumen now has base units of 
candela * steradian = candela * radian^2. 


# Types and Templates
Dim fundamentally works with a class template of `quantity`.  This in turn depends on three
parameters:

  1. The scalar type (`double` by default)
  2. The unit
  3. The system (e.g. SI)
  
(The system is effectively a tag type to prevent quantities from different systems from being
mixed.  Dim only ships with the SI system, so this only matters if you are building your own
system.)

The unit is the core of Dim.  It is a set of eight short int template parameters representing
the seven fundamental dimension of the standard model plus an angle dimension, but these are
treated as template parameters, not class members.  All operations on the unit are performed
at compile time, so a `quantity` uses exactly as many bytes as its scalar type.  The unit type
is actually a separate template class that acts like a quantity with scalar value of one, though
Dim goes to some length to hide the unit type from you.  For the SI system, the fundamental 
dimensions have unit types defined as `meter_`, `second_`, `radian_`, and so on, while the 
quantities do not have the trailing underscore.

## Defining Custom Quantities
You can define new quantities simply by using the C++ decltype operator. For example, suppose
we wanted to represent a screw pitch.  We could do
```cpp
using Pitch = decltype(meter/radian);
```
and then Pitch becomes a convenient alias. See the code `example/advanced.cpp`. Alternatively,
you can work with the template parameters
```cpp
using Pitch = quantity<unit_divide_t<Length::unit, Angle::unit>, double>;
```
The metafunctions `unit_multiply_t` and `unit_pow_t` are defined similarly.

## Defining Custom Literal Formatters
Dim ships with literal formatters based on the NIST standard. But often other formatters are
useful for your domain.  Suppose we want angular degrees to have the format `_deg`.  We can 
define
```cpp
DIM_MAKE_LITERAL(deg, si::degree)
```
so that `1_deg` and `1.2_deg` are both recognized as Angle constexprs. See the code `example/advanced.cpp`.

## Type Definitions Attached to Quantity
Every quantity class holds the typedefs 

* `scalar` -- the scalar type, e.g. double
* `unit` -- the underlying unit array (it is a type all of whose instances are the same)
* `type` -- the type of this quantity
* `dimensionless` -- the unit in this system that has zero for all dimension powers

## The `dimensionless_cast` Escape Hatch
Dim allows you to "escape" from the confines of the library at will.  The magnitude of a quantity
is available via `dimensionless_cast()`.  Accessing the magnitude can be useful when 
serializing data.

## Guard Values, NaN, and `bad_quantity`
Dim uses NaN as a guard value in several places. You can construct a "bad" version of a type
via
```cpp
Length its_bad = Length::bad_quantity();
```
and you can check for badness with `is_bad()`.  See [Bad Quantities and NaN](#Bad-Quantities-and-NaN) 
for details.


## Math with Quantities
Quantities overload the arithemetic operators "+", "-", "*", "/", the assignment operators 
"=", "+=", "-=", "*=", "/=", and the full set of comparison operators. In short, they behave just 
like doubles except for the restrictions regarding dimensions.  In addition, the following overoads
are defined:

* `abs()`: Computes the absolute value, returning the same type of quantity.
* `sqrt()`: Computes the square root *provided* all of the dimensions are divisible by two.
* `pow<n>()` : Computes the nth power where n is an integer.
* `ratpow<p,q>()` Computes the quantity to the p/q power, provided all of the resulting dimensions
  are integers.

In addition, Angle types have the related trigonometric functions defined:

* `double sin(Angle const& q)`
* `double cos(Angle const& q)`
* `double tan(Angle const& q)`
* `Angle asin(double const& x)`
* `Angle acos(double const& x)`
* `Angle atan(double const& x)`
* `Angle atan2(double const& x, double const& y)`
* `template<class Q> Angle atan2(Q const& x, Q const& y)`


## Understanding Errors

Dim uses static assertions to improve the legibility of most compile errors. As Dim is a template library, there's
a lot of "angry template error" messages to wade through, but you should be able to find a more helpful error in 
the spew, such as
```
... error: static assertion failed: Length dimensions do not match.
```

In some cases, the compiler will give errors of the form
```
... error: conversion from ‘quantity<unit<1,[...],[...],[...],[...],[...],[...],[...],[...]>,[...]>’ to non-scalar type ‘quantity<unit<2,[...],[...],[...],[...],[...],[...],[...],[...]>,[...]>’ requested
    8 |     si::Area A2 = L;
```
To decode this, we've tried to assign a length to an area.  You can see in the template that the RHS has 
a length dimension of 1 while the LHS has a length dimension of 2.

## Dynamic Quantities

Dim has a second quantity type, the `dynamic_quantity`. These types store the
unit as a `dynamic_unit` member, not implicitly as part of their type. As such,
they require more memory and have worse performance.  Dynamic quantities support
the same operator overloading as static quantities, but with one different:
`operator=` will change the type of the left-hand side as needed. For example
```
si::dynamic_quantity q1(5.0_m);
si::dynamic_quantity q2(12.0 * si::meter/si::second);
q1 = q2; // q1 now has speed dimensions
```
