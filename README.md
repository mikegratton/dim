# Dim: A Library for Compile-time Dimensional Analysis

## Overview

When working with dimension quantities like length and time in code,
two major problems arise:

    1. Different authors expect different units (feet instead of meters, say)
    2. Function arguments are initialized wrong at the call site
    
For (2), I mean
```
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
that rely on nondimensional values, it can still be helpful to read and write dimensional 
values at the start and end of the code.

In addition, one often must print a quantity to a string or parse a string into a 
quantity. Dim provides extensive IO support. The main path uses the iostream facet/locale
system to stash your preferred formats and have them be applied wherever `operator<<` is 
used.  However, for programs that do not wish to use iostreams, Dim provides a simple system
based on c-strings for IO.

## Quickstart
Here's a basic annotated program
```
#include <cassert>
#include <iostream>
#include <dim/si.hpp> // 1
### Rationale
using namespace dim::si; // 2
using namespace dim::si::literal; // 3

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
  
  2. Not required, but move the dim::si symbols into this namespace
  
  3. To use literals (see 4), you need to have this `using` statement
  
  4. Construct a Time quantity using a literal formatter. See `dim/si/literal.hpp` for 
     info on how to make your own.
     
  5. Make a Speed quantity using arithmetic operators.  `my_speed` is actually set to 
     a value in m/s, but *it isn't necessary to know this* if you use Dim operators.
     
  6. This prints "In 3_s, an object traveling at speed 0.6096_m_s^-1 travels 1.8288_m"
  
You can find this code in `example/quickstart.cpp`.

In function  `compute_distance`, the compiler has checked that e.g. Time*Speed 
results in units of Speed.  If we instead tried to write a function body of
```
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


## Design

### Rationale

### Understanding Dimensional Analysis


### Types and Templates
Dim fundamentally works with a class template of `quantity`.  This in turn depends on three
parameters:

  1. The scalar type (double for the SI system)
  2. The unit
  3. The system (e.g. SI)
  
The system is effectively a tag type to prevent quantities from different systems from being
mixed.  Dim only ships with the SI system, so this only matters if you are building your own
system.

The unit is the core of Dim.  It is a set of eight short int template parameters representing
the seven fundamental dimension of the standard model plus an angle dimension (see below).
These are template parameters, not class members.  All operations on the unit are performed
at compile time, and a quantity has a size exactly equal to its scalar type.  The unit type
is actually a separate template class that acts like a quantity with scalar value of one, though
Dim goes to some length to hide the unit type from you.  For the SI system, the fundamental 
dimensions have unit types defined as `meter_`, `second_`, `radian_`, and so on, while the 
quantities do not have the trailing underscore.

#### Defining Custom Quantities
You can define new quantities simply by using the C++ decltype operator. For example, suppose
we wanted to represent a screw pitch.  We could do
```
using Pitch = decltype(meter/radian);
```
and then Pitch becomes a convenient alias. See the code `example/advanced.cpp`.

#### Defining Custom Literal Formatters
Dim ships with literal formatters based on the NIST standard. But often other formatters are
useful for your domain.  Suppose we want angular degrees to have the format `_deg`.  We can 
define
```
inline constexpr Angle operator "" _deg(long double x) { return static_cast<double>(x)*degree; } \
inline constexpr Angle operator "" _deg(unsigned long long x) { return static_cast<double>(x)*degree; } 
```
so that `1_deg` and `1.2_deg` are both recognized as Angle constexprs. See the code `example/advanced.cpp`.


### NIST SP 811 Standard and the Angle Unit
[The National Institute of Standards and Technology Special Publication 881](https://www.nist.gov/pml/special-publication-811)
is the most complete guide to SI units. Wherever possible, Dim uses the NIST SP 881 unit names, abbreviations,
and so on. Note that in this standard, plurals are generally avoided, so it is "meter" and not "meters."

There is one major exception: Dim considers Angle to be a fundamental dimension. This choice resolves
a lot of ambiguities in the standard system.  For instance, the bedevilment caused by wondering if
a frequency is Hz or is a *rotational frequency* is resolved. These have different units: Hz is 1/s while
rotational frequency is rad/s. Other notable changes are that the units of torque ang luminous flox.
The units of torque are N m / rad, so that
if you apply this torque through a full revolution, you see the work done is (N m / rad) * (2 * pi * rad) = 2 * pi * W.
Likewise the  lumen now has base units of candela * steradian = candela * radian^2.

### The `value` Escape Hatch
Dim allows you to "escape" from the confines of the library at will.  The magnitude of a quantity
is stored as a public scalar `value`.  Accessing the magnitude in this way means potentially
having dimensional mismatch (that is, the library cannot check you), but allows for type-punning
of the form
```
double came_from_elsewhere = 5.0;
Length* l = reinterpret_cast<Length*>(&came_from_elsewhere);
```
This is very useful when serializing or deserializing data, though it may cause difficulty at
your next code review.

### Understanding Errors

TODO

# Input and Output

## Formatters, Facets, and Locales

The formatter is a simple class that associates a string symbol with information on how 
to nondimensionalize a quantity. For example
```
dim::formatter<si::Angle> degree_formatter("deg", si::degree);
```
will format angles as degrees. The template parameter is the quantity
type (`si::Angle`), the first argument is the symbol string (`deg`), and the second argument gives
what you would multiply 1.0 by to obtain a quantity of the indicated symbol (`si::degree`).  There's
an optional third argument that is used for temperature conversion as an additive offset:
```
dim::formatter<si::Temperature> fahrenheit_format("F", 5./9.*kelvin, 255.37*kelvin);
```
Hey, no one ever said thermodynamics was easy.  

Formatters can be used directly, providing `input` and `output` methods.  Input takes the form
```
si::Angle x = degree_formatter.input(90.0);
```
while output is
```
dim::formatted_quantity<double> fmt = degree_formatter.output(2_rad);
```
which divides the output into a scalar (double) part `value()` and a `char const*` symbol part `symbol()`.
This allows you to do
```
printf("My angle is %g_%s\n", fmt.value(), fmt.symbol());
```
or put the result into structured XML, etc.  If you are using streams, you can just do
```
std::cout << "My angle is " << degree_formatter.output(2_rad) << "\n";
```

Dim taps into the facet/locale system of std::iostream to make these formatters available 
transparently to streaming operators. The facet allows `operator<<` and `operator>>` to look 
up the appropriate formatter for the quantity and apply it. To use this system, you need to
install it into the global locale.  You probably want to adjust the format to your tastes.  
Here's an example of setting up angles to be output in degrees by default:
```
#include <dim/si.hpp>
namespace si = ::dim::si;

int main(int argc, char** argv)
{
    // Let's format angles as degrees
    auto* facet = si::system::make_default_facet(); // obtain the default si facet    
    dim::formatter<si::Angle> degree_formatter("deg", si::degree);
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
you call `facet->output_formatter()`, you are replacing the default format.  If you want to override
the format only in certain places, you can construct and use the formatter as shown above.

### Basic Format
But what if there's no format? Well, Dim has a basic fallback output format. A symbol is associated
with each dimension so a unit symbol can be reconstructed. This is correct, but it isn't pretty.
Thus suppose we had a torque we wanted to print:
```
si::Torque T = 3_N * 2_m / 1.2_rad;
std::cout << "Torque is " << T << "\n";
```
If there's nothing in the facet about `si::Torque`, the output will be "5_rad^-1_kg_m^2_s^-2".

## Parsing Quantities

Input is a different beast. One interesting part is that because Dim is strongly typed, we know
the desired destination quantity type.  Really we need to see that the string can be parsed to a 
matching dimension. For example,
```
si::Angle the_angle;
std::cout << "Enter an angle: ";
std::cin >> the_angle;
```
If the user enters is `3 rad`, all is well. If the enters `12 ft`, we've got a problem.  Dim doesn't
use exceptions. Instead `the_angle` will be set to a "bad" quantity -- a silent `NaN`.  The test
`the_angle.is_bad()` can check that a valid input was recieved. 

### Stream-based Parsing
What formatters are available for what types? Well, the facet maintains a map of maps for input.  First,
since we know the destination quantity type, it looks up the map for that type. The resulting map is
then indexed by the symbol string. If an exact string match is found, that formatter is used to parse
the quantity.  This provides a flexible system where you can provide formatters for whatever 
domain-specific conventions you work with.  For instance, here's the default map that Dim ships 
with for lengths:
```
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
Notice that `m` and `cm` and so on aren't there because they are handled by the fallback parser
below. (SI units are generally easy to parse). Instead, here are conventions suggested by NIST SP 811.
But suppose for your domain, you wanted to accept "feet" for "foot" and "nm" for nautical mile (yikes,
symbol clash with nanometer -- but you know what your users expect).  You could add these to the 
facet via
```
auto* facet = si::system::make_default_facet(); // obtain the default si facet    
facet->input_formatter(formatter<Length>("feet", foot));
facet->input_formatter(formatter<Length>("nm", nautical_mile));
```
Because the map is consulted first, you've overridden the meaning of "nm" for your program.

### Fallback Parser

When a symbol isn't recognized, Dim falls back to a Flex/Bison built parser for SI types.  This
parser understands the SI prefixes and symbols defined in NIST SP 811. The parser recognizes 
`_`, `*` and space as multiplication, `/` as division, and `^` as exponentiation, as well as
parentheses.

The symbol tables differ slightly from the standard:

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
    
where mu (&#956;) has been replaced by "u".  The unit symbols are 

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
| Pa   | pascale|
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
| a    | acre|
| bar  | bar |

where Omega (&#937;) has been replaced by "R". 

# Advanced Topics

## Metaprograming with Quantities
One pitfall for C++ class templates is that each instantiation is a totally separate class at 
run time from every other one.  Thus `si::Length` and `si::Time` are no more related than
`std::string` and `double`.  This is often painful for code that wants to handle these types in
a uniform way.  To aid with this, Dim provides a macro `DIM_IS_QUANTITY` that works with the C++ 
Substitution Failure is not an Error (SFINAE -- this language has the *worst* jargon).  It works 
like this
```
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
```
Length L = 2_m;
double v1 = L.value; // 1
double v1 = L / meter; // 2
double v2 = L / meter_; // 3
```
(1) just uses our knowledge that lengths are stored in meters.  It is a bit unsafe, however.  (2)
does just what it says, provide the length of L in meters. However, since `meter` is itself a Length 
quantity, this actually performs a floating point division (2.0/1.0).  (3) uses the specially defined
*unit* types (these are the same as the quantities but have a trailing underscore).  The `operator/`
action here understands that a unit has, well, *unit* value, so avoids the division.  In fact,
at runtime, (1) and (3) produce the same assembly code.

## Fractional Dimensions

Dim does not support fractional dimension like "m^1/2" that are used in some domains.  Supporting
these really complicates the metaprogramming and makes your program compile slower.  You can typically
work with e.g. squared quantities to obtain integer dimensions. (For fractal dimensions, why are 
you using this library at all?) If this is too much of a burden, the 
[boost::units](https://www.boost.org/doc/libs/1_80_0/doc/html/boost_units.html)
library provides this feature.


## Bad Quantities and NaN 

Dim uses an expanded notion of a "bad" floating point number that includes numbers that are
Inf, -Inf, and NaN in IEEE 754 floating point. The rationale is that on embedded systems (ARM in 
particular), you need to compile `-Ofast` to obtain access to SIMD instructions. When you do this,
`std::isnan()` always evaluates to false.  Dim uses its own `is_bad` function to work around this.
The trade-off is that algorithms relying on correct behavior with Inf will see values as "bad".
But such algorithms are usually limited to specialized scientific codes rather than the more general 
robotics and engineering codes Dim is intended for.
