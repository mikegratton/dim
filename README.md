# Dim: A Library for Compile-time Dimensional Analysis

* [GitHub Repository](https://github.com/mikegratton/dim)
* [Documentation](https://mikegratton.github.io/dim/)


## Overview

When working with dimension quantities like length and time in code,
two major problems arise:

 1. Different authors expect different units (feet instead of meters, say)
 2. Function arguments are initialized wrong at the call site
    
For (2), I mean
```cpp
double compute_distance(double time_s, double speed_mps)
{
    return time_s*speed_mps;
}
// ...
double dist = compute_distance(speed, time); // Oops...
```
Dim solves these problems by using the C++ template feature to make each quantity
a different type. This shifts the burden of dimension checking and many conversions
to *compile time* rather than run time, making it very efficient.  Dim is aimed at
robotics and engineering applications, where calculations with dimensional quantities and 
especially quantities of *different dimensions* are common.  Still, for scientific codes
that typically work with nondimensional values, it can still be helpful to read and write 
dimensional values at the start and end of a calculation.

In addition, one often must print a quantity to a string or parse a string into a 
quantity. Dim provides extensive IO support through a layered approach. The easiest to use path 
uses the iostream facet/locale system to stash preferred formats and have them be applied 
wherever `operator<<` is used. This is as simple as

However, for programs that do not wish to use iostreams, Dim provides a simple system based on 
c-strings for IO.

## History

### v1.2

Bug fixes:
* Fixed issue stream extraction for non-whitespace separated quantity strings.

### v1.1

Bug fixes:
* Fixed issue with compilation in C++11 mode

Features:
* Use a custom flat map for formatters. These make setting and changing formatters less verbose,
as well as improving runtime performance by some 30%.
* Add unit index to quantity and unit. This is a 64-bit number that uniquely represents each unit.
* Made major improvements to the Bison-based parser. The new parser does not copy any strings and 
  is 10% more efficient.

Changes:
* Due to the parser changes, the are ("a") and the hectare ("ha") are no longer recognized by the 
fallback parser. The format map for area types has gained these quantities.


### v1.0

Initial release.

## Quickstart
Here's a basic annotated program
```cpp
#include <cassert>
#include <iostream>
#include <dim/si.hpp> // 1
using namespace si; // 2
using namespace si::literal; // 3

Length compute_distance(Time t, Speed v) { return t*v; }

int main(int argc, char** argv)
{
    Time my_time = 3_s; // 4
    Speed my_speed = 2.0 * foot/second; // 5
    Length my_distance = compute_distance(my_time, my_speed);
    std::cout << "In " << my_time << ", an object traveling at speed " << my_speed 
              << " travels " << my_distance << "\n"; // 6
    assert(sizeof(my_distance) == sizeof(double)); // No fluff.
    return  0;
}
```
Notes:

  1. Use the SI system with stream operators
  
  2. Not required, but move the si:: symbols into this namespace
  
  3. To use literals (see 4), you need to have this `using` statement
  
  4. Construct a Time quantity using a literal formatter. See `dim/si/literal.hpp` for 
     info on how to make your own.
     
  5. Make a Speed quantity using arithmetic operators.  `my_speed` is actually set to 
     a value in m/s, but *it isn't necessary to know this* if you use Dim operators.
     
  6. This prints "In 3_s, an object traveling at speed 0.6096_m_s^-1 travels 1.8288_m"
  
You can find this code in `example/quickstart.cpp`.

In function  `compute_distance`, the compiler has checked that e.g. Time*Speed 
results in units of Speed.  If we instead tried to write a function body of
```cpp
Length compute_distance(Time t, Speed v) { return 0.5*t*t*v; }
```
the program would not compile.  The error is
```
../example/quickstart.cpp: In function ‘dim::si::Length compute_distance(dim::si::Time, dim::si::Speed)’:
../example/quickstart.cpp:8:58: error: could not convert ‘dim::operator*<dim::quantity<dim::unit<1, -1, 0, 0, 0, 0, 0, 0, dim::si::system>, double> >(dim::operator*<dim::quantity<dim::unit<0, 1, 0, 0, 0, 0, 0, 0, dim::si::system>, double> >(dim::operator*(5.0e-1, t), t), v)’ from ‘quantity<unit<[...],1,[...],[...],[...],[...],[...],[...],[...]>,[...]>’ to ‘quantity<unit<[...],0,[...],[...],[...],[...],[...],[...],[...]>,[...]>’
    8 | Length compute_distance(Time t, Speed v) { return 0.5*t*t*v; }
```
More on what all this template business is is below, but suffice to say that Length does not 
match the dimensions of Speed * Time * Time.

## Building Dim 

Dim is written in C++11 and has no dependencies. If C++14 or higher is available, a few extra
`constexpr` features are enabled.

The easiest way to use Dim is to put the code in a subdirectory of your project and add 
```
add_subdirectory(dim)
```
to your `CMakelists.txt` file. This will give you a target `dim` that you can depend on.  There 
isn't much to compile in Dim, so the additional build time should be small.

Alternatively, you can run the install target to install Dim to a directory per usual cmake practice.

## Design

### Understanding Dimensional Analysis

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


### Types and Templates
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

#### Defining Custom Quantities
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

#### Defining Custom Literal Formatters
Dim ships with literal formatters based on the NIST standard. But often other formatters are
useful for your domain.  Suppose we want angular degrees to have the format `_deg`.  We can 
define
```cpp
inline constexpr Angle operator "" _deg(long double x) { return static_cast<double>(x)*degree; } \
inline constexpr Angle operator "" _deg(unsigned long long x) { return static_cast<double>(x)*degree; } 
```
so that `1_deg` and `1.2_deg` are both recognized as Angle constexprs. See the code `example/advanced.cpp`.


### NIST SP 811 Standard and the Angle Unit
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

### Type Definitions Attached to Quantity
Every quantity class holds the typedefs 

* `scalar` -- the scalar type, e.g. double
* `unit` -- the underlying unit array (it is a type all of whose instances are the same)
* `type` -- the type of this quantity
* `dimensionless` -- the unit in this system that has zero for all dimension powers

### The `dimensionless_cast` Escape Hatch
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


### Math with Quantities
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


### Understanding Errors

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

# Input and Output

In Dim, "input and output" is used to mean deserialization and serialization from text. 

## Formatters, Facets, and Locales
Input/output in Dim rests on the `formatter` class. The formatter is a simple class that associates 
a string "symbol" with information on how to nondimensionalize a quantity. For example
```cpp
si::formatter<si::Angle> degree_formatter("deg", si::degree);
```
will format angles as degrees. The template parameter is the quantity
type (`si::Angle`), the first argument is the symbol string ("deg"), and the second argument gives
what you would multiply 1.0 by to obtain a quantity of the indicated symbol (`si::degree`).  There's
an optional third argument that is used for temperature conversion as an additive offset:
```cpp
si::formatter<si::Temperature> fahrenheit_format("F", 5./9.*kelvin, 255.37*kelvin);
```
Hey, no one ever said thermodynamics was easy.  

Formatters provide `input` and `output` methods.  Input takes the form
```cpp
si::Angle x = degree_formatter.input(90.0);
```
while output is
```
si::formatted_quantity<double> fmt = degree_formatter.output(2_rad);
```
The `formatted_quantity` divides the output into a scalar (double) part `value()` and a `char const*` 
symbol part `symbol()`. This allows you to do
```cpp
printf("My angle is %g_%s\n", fmt.value(), fmt.symbol());
```
or put the result into structured XML, etc.  An overload for `operator<<` allows for interaction with
`ostream` objects:
```cpp
std::cout << "My angle is " << degree_formatter.output(2_rad) << "\n";
```

Formatters are nice, but keeping track of them is often a bother. Dim taps into the facet/locale system of 
`std::iostream` to make these formatters available transparently to stream operators. The facet allows 
`operator<<` and `operator>>` to look up the appropriate formatter for the quantity and apply it. To use 
this system, you need to install the Dim facet into the global locale.  You may want to adjust 
the format to match expectations in your domain.  Here's an example of setting up angles to be output in 
degrees by default:
```cpp
#include <dim/si.hpp>
namespace si = ::dim::si;

int main(int argc, char** argv)
{
    // Let's format angles as degrees
    auto* facet = si::system::make_default_facet(); // obtain the default si facet    
    si::formatter<si::Angle> degree_formatter("deg", si::degree);
    facet->output_formatter(degree_formatter); // Set a new output format for angles    
    si::add_to_global_locale(facet); // Install the updated facet in the global locale
    
    si::Angle angle = M_PI*0.5 * si::radian;    
    std::cout << "Angle is " << angle << "\n";
    return 0;
}
```

What happens if the facet doesn't exist in the locale, or if the facet doesn't have a formatter
for our case? Dim provides fallback options in these cases described below.

## Printing Quantities

Printing takes advantage of the fact that each quantity can only have one default way to print it.
Thus the facet maintains a map indexed by the quantity's dimension to the formatter to use.  When
you call `facet->output_formatter()`, you are replacing the default format.  (If you want to override
the format only in certain places, you can construct and use the formatter as shown above.)

### Basic Format
But what if there's no format? Well, Dim has a basic fallback output format. A symbol is associated
with each dimension in a system, so a unit symbol can be reconstructed. This is correct, but it isn't 
usually very pretty. Thus suppose we had a torque we wanted to print:
```cpp
si::Torque T = 3_N * 2_m / 1.2_rad;
std::cout << "Torque is " << T << "\n";
```
If there's no output formatter for `si::Torque`, the output will be "5_rad^-1_kg_m^2_s^-2".

You can access these basic printing functions directly with `print_unit()` and `print_quantity()`. The former
prints the unit alone, while the latter will print the scalar and the unit joined by '_'. 

## Parsing Quantities

Input is a different beast. Because Dim is strongly typed, we know the desired quantity type.  The task is
to verify that the input string can be parsed to a matching dimension, then apply the conversion factor for
that dimension to the scalar part. For example,
```cpp
si::Angle the_angle;
std::cout << "Enter an angle: ";
std::cin >> the_angle;
```
If the user enters is `3 deg`, Dim determines that the scalar part is 3 and the symbol is 3*pi/180 rad -- an 
angle type. All is well. If the user enters `12 ft`, this is a valid quantity, but not a valid angle.  As Dim 
doesn't use exceptions, we treat incompatible input dimensions as a "bad quantity" -- a silent `NaN`.  The test
`the_angle.is_bad()` can check if an invalid input was received. 

### Stream-based Parsing
The facet maintains a map from input type to symbol for each quantity type indexed by the symbol string.
If the input unit string is an exact match to a map entry, that formatter is used to parse
the quantity.  This provides a flexible system where you can provide formatters for whatever 
domain-specific conventions you work with.  Dim ships with format maps for many common quantities. These
are derived from the NIST suggestions. For instance, here's the default length map:
```cpp
    static const format_map<Length> s_known {
        {"in",   formatter<Length>("in", inch) },
        {"inch", formatter<Length>("inch", inch)},
        {"ft",   formatter<Length>("ft", foot)},
        {"foot", formatter<Length>("foot", foot)},
        {"yd",   formatter<Length>("yd", yard)},
        {"yard", formatter<Length>("yard", yard)},
        {"mi",   formatter<Length>("mi", mile)},
        {"mile", formatter<Length>("mile", mile)},
        {"nmi",  formatter<Length>("nmi", nautical_mile)},
        {"nautical_mile", formatter<Length>("nautical_mile", nautical_mile)}
    };
```
Note that "m" and related SI ("km", "mm", etc) units aren't included because they are handled by the fallback parser
below -- SI units are generally easy to parse. But suppose for your domain, you wanted to accept "feet" for 
"foot" and "nm" for nautical mile (yes, that's a symbol clash with nanometer -- but you know what your users 
expect).  You could add these to the facet via
```cpp
auto* facet = si::system::make_default_facet(); // obtain the default si facet    
facet->input_formatter(formatter<Length>("feet", foot));
facet->input_formatter(formatter<Length>("nm", nautical_mile));
```
Because the map is consulted before the fallback parser is called, you've overridden the meaning of "nm" for 
your program.

Dim uses a custom flat map to store the formatters. The maps are sorted upon each insertion, but are substantially
faster to search during normal operation of the facet as the data is contiguous in memory.

Dim provides `si::to_string()` and `si::from_string()` to use the facet without stream-based operations.  Additionally,
`parse_quantity()` provides an interface that doesn't use `std::string` or the facet.

### Fallback Parser

When a symbol isn't recognized, Dim falls back to a Bison-built parser for SI types.  This
parser understands (almost all of) the SI prefixes and symbols defined in NIST SP 811. The parser recognizes 
`_`, `*` and space as multiplication, `/` as division, and `^` as exponentiation, as well as
parentheses. 

The symbol tables differ very slightly from the standard, avoiding non-ASCII characters.

| Prefix | Magnitude |
|--------|-----------|
| y | 1e-24 |
| z | 1e-21 |
| a | 1e-18 |
| f | 1e-15 |
| p | 1e-12 |
| n | 1e-9 |
| u*| 1e-6 |
| m | 1e-3 |
| c | 1e-2 |
| d | 1e-1 |
| Y | 1e24 |
| Z | 1e21 |
| E | 1e18 |
| P | 1e15 |
| T | 1e12 |
| G | 1e9 |
| M | 1e6 |
| k | 1e3 |
| h | 1e2 |
    
*mu (&#956;) has been replaced by "u". 

|Symbol|Name|
|------|----|
| m    | meter|
| s    | second|
| rad  | radian|
| g    | gram|
| K    | kelvin|
| mol  | mole|
| A    | ampere|
| cd   | candela|
| Hz   | hertz|
| sr   | steradian|
| N    | newton|
| Pa   | pascal|
| J    | joule|
| W    | watt|
| C    | coulomb|
| V    | volt|
| F    | farad|
| R*   | ohm|
| S    | siemens|
| Wb   | weber|
| T    | tesla|
| H    | henry|
| Im   | lumen|
| Ix   | lux  |
| Bq   | becquerel   |
| Sv   | sievert    |
| kat  | katal|
| L    | liter|
| eV   | electron volt|
| bar  | bar |
| -- | are**|

*Omega (&#937;) has been replaced by "R". 

**The are (symbol "a") has been excluded as it makes parsing the units string ambiguous (is "Pa" a pascal or is it a petaare?).  

The Bison-generated C++ code is part of the Dim project so that you don't need Bison installed to build or use Dim. If you wish to
modify the grammar, the provided Makefile will regenerate the code. The parser has been designed to avoid memory allocation or string 
copies. 

## Simple and Complete Parsing

The `parse_quantity()` function accesses the default unit symbol maps and uses the fallback parser where 
required. You call it like so:
```cpp
double scale = ...; // Taken from your data source
char const* unit_string = ... ; // Taken from your data source
si::Force f;
if (parse_quantity(f, scale, unit_string)) {
    // Use f
}
```
This avoids the use of `std::string` or `iostream` while still having access to both layers of the input code. 
The default symbol map is consulted first, followed by the fallback parser if that map doesn't contain the symbol.


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
will have a member `value`. A C++17 `if constexpr` functionality may be added at a later date.


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
(1) just uses our knowledge that lengths are stored in meters.  It doesn't document this fact very well, 
however  (2)
does just what it says, provide the length of L in meters. However, since `meter` is itself a Length 
quantity, this actually performs a floating point division (2.0/1.0).  (3) uses the specially defined
*unit* types (these are the same as the quantities but have a trailing underscore).  The `operator/`
action here understands that a unit has, well, *unit* value, so avoids the division.  In fact,
at runtime, (1) and (3) produce the same assembly code.

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

# Appendix A: Default Quantity Input Symbols
For non-SI quantities, Dim provides a default override map for typical unit abbreviations.
See [Stream-based Parsing](#Stream-based-Parsing) for information on changing these lists.
Note that standard SI notation doesn't appear below as the parser handles it just fine.

## Temperature
* "F" : Fahrenheit
* "R" : Rankine
* "C" : Celsius

## Length
* "in" : Inch
* "inch" : Inch
* "ft" : Foor
* "foot" : Foot
* "yd" : Yard
* "yard" : Yard
* "mi" : Mile
* "mile" : Mile
* "nmi" : Nautical Mile
* "nautical_mile" : Nautical Mile

## Time
* "min" : Minute
* "h" : Hour
* "hr" : Hour
* "minute" : Minute
* "hour" : Hour


## Mass
* "oz" : Ounce (mass)
* "lb" : Pound (mass)
* "lbm" : Pound (mass)
* "slug" : Slug
* "ounce" : Ounce (mass)
* "pound" : Pound (mass)
* "pound_mass" : Pound (mass)


## Angle
* "radian" : Radian
* "deg" : Degree
* "mil" : Mil
* "mrad" : Milliradian
* "milliradian" : Milliradian
* "turn" : Turn 
* "'" : Minute (angle)
* "min" : Minute (angle)
* "minute" : Minute (angle)
* "\"" : Second (angle)
* "s" : Second (angle)
* "second" : Second (angle)


## SolidAngle
* "steradian" : Steradian
* "sp" : Spat
* "spat" : Spat


## Force
* "dyn" : Dyne
* "lb" : Pound (force)
* "lbf" : Pound (force)
* "pound" : Pound (force)
* "pound_force" : Pound (force)

## Pressure
* "lbf/in^2" : Pound (force) per Square Inch
* "lbf_in^-2" : Pound (force) per Square Inch
* "lb/in^2" : Pound (force) per Square Inch
* "lb_in^-2" : Pound (force) per Square Inch
* "pounds_per_inch2" : Pound (force) per Square Inch
* "pounds_per_square_inch" : Pound (force) per Square Inch
* "torr" : Torr
* "atm" : Standard Atmosphere
* "atmosphere" : Standard Atmosphere

## Energy
* "kW_hr" : Kilowatt Hour
* "kW*hr" : Kilowatt Hour
* "kWhr" : Kilowatt Hour
* "erg" : Erg
* "foot_pound" : Foot Pound (force)
* "ft_lb" : Foot Pound (force)
* "ft_lbf" : Foot Pound (force)
* "BTU" : British Thermal Unit

## Power
* "hp" : Horsepower

## Area
* "acre" : Acre
* "sq_mi" : Square Mile
* "mi^2" : Square Mile
* "mile2" : Square Mile
* "sq_ft" : Square Foot
* "ft^2" : Square Foot
* "foot2" : Square Foot
* "a" : are
* "ha" : hectare

## Volume
* "cc" : Cubic Centimeter
* "gal" : Gallon
* "gallon" : Gallon
* "acre_ft" : Acre Foot
* "acre_foot" : Acre Foot
* "cu_ft" : Cubic Foot
* "ft^3" : Cubic Foot
* "cubic_foot" : Cubic Foot
* "cu_in" : Cubic Inch
* "in^3" : Cubic Inch
* "cubic_inch" : Cubic Inch
* "cu_yd" : Cubic Yard
* "yd^3" : Cubic Yard
* "cubic_yard" : Cubic Yard

## FlowRate
* "gal/s" : Gallon per Second
* "gal/min" : Gallon per Minute
* "meter3/second" : Cubic Meter per Second
* "liter/second" : Liter per Second
* "gallon/second" : Gallon per Second
* "gallon/minute" : Gallon per Minute

## Speed
* "mps" : Meter per Second
* "kph" : Kilometer per Hour
* "mph" : Mile per Hour
* "miles_per_hour" : Mile per Hour
* "knot" : Knot
* "kn" : Knot
* "kt" : Knot
* "ft/s" : Foot per Second
* "feet_per_second" : Foot per Second

## Acceleration
* "ft/s^2" : Foot per Squared Second
* "feet_per_second2" : Foot per Squared Second

## AngularRate
* "deg/s" : Degree per Second
* "degrees_per_second" : Degree per Second
* "radians_per_second" : Radian per Second

## AngularAcceleration
* "deg/s^2" : Degree per Squared Second
* "degrees_per_second2" : Degree per Squared Second
* "radians_per_second2" : Radian per Squared Second