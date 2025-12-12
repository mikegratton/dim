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

In addition, one often must print a quantity to a string or parse a string into
a quantity. Dim provides extensive IO support through a layered approach. The
easiest to use path uses the locale/facet system to stash preferred formats and
have them be applied wherever `operator<<`, `std::format`, or `to_string` is
used. 

## History

### v2.0.1

Bug fixes:
  * Fixed malformed quantity operators.
  * Fixed missing dynamic_quantity operators.

### v2.0

Bug fixes:
  * Fixed scale factors with temperature formatters.
  * Fixed scalar/unit string scanner state machine cases.

Features:
  * Added UTF-8 support in unit string symbols.
  * Added UTF-8 support in SI unit parser.
  * Reduced the compiled library size by two thirds.
  * Expanded the list of accepted input symbols.
  * Added additional `constexpr` methods when c++14 is detected.
  * Using `to_chars` when c++17 is detected.
  * Added support for `std::format` when c++20 is detected.
  * Added static analysis and sanitizer builds.
  * Expanded the unit test suite.

### v1.2

Bug fixes:
* Fixed issue stream extraction for non-whitespace separated quantity strings.
* Fixed calling of `symbol()` in io.hpp to avoid some issues that can occur when in a source file with CUDA code.
* Removed stray c++14ism

Features:
* Simplification of templates has reduced compile times slightly

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

Dim supports C++11 and has no dependencies. If C++14 or higher is available, a
few extra `constexpr` features are enabled. If C++17 is available, the
`from_chars`/`to_chars` functions are used for faster
serialization/deserialization. If C++20 is available, `std::format` support is
enabled.

The easiest way to use Dim is to put the code in a subdirectory of your project and add 
```
add_subdirectory(dim)
```
to your `CMakelists.txt` file. This will give you a target `dim` that you can depend on.  There 
isn't much to compile in Dim, so the additional build time should be small.

Alternatively, you can run the install target to install Dim to a directory per usual cmake practice.

## CMake Options

| Option         | Default | Effect                                                                                                 |
|----------------|---------|--------------------------------------------------------------------------------------------------------|
| DIM_STREAM     | ON      | Enable `ostream` and `istream` support                                                                 |
| DIM_STRING     | ON      | Enable `std::string` support (to_string, from_string)                                                  |
| DIM_EXCEPTIONS | OFF     | Deserialization and dynamic_quantity operators may throw exceptions when dimensions are not compatible |

