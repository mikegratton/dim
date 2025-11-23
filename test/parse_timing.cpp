#include <chrono>
#include <iostream>
#include "dim/io_format.hpp"
#include "dim/si.hpp"
#include "doctest.h"

using namespace dim::si;

TEST_CASE("LookupTiming" * doctest::skip())
{
    std::string literal = "123_Mg*m/s^2";
    Force force;
    CHECK(dim::from_string(force, literal));
    CHECK(force / newton == doctest::Approx(123e3));
    int N = 100000;
    auto start = std::chrono::system_clock::now();
    for (int i = 0; i < N; i++) { dim::from_string(force, literal); }
    auto stop = std::chrono::system_clock::now();
    double elapsed = (stop - start) / std::chrono::nanoseconds(1) * 1e-9;
    std::cout << "Parsed " << N << " quantities with full parser in " << elapsed << ", " << N / elapsed << " parse/s\n";

    start = std::chrono::system_clock::now();
    char const* unit_literal = "Mg*m/s^2";
    for (int i = 0; i < N; i++) { dim::detail::parse_standard_rep<double, dim::si::system>(unit_literal, -1); }
    stop = std::chrono::system_clock::now();
    elapsed = (stop - start) / std::chrono::nanoseconds(1) * 1e-9;
    std::cout << "Parsed " << N << " quantities from fallback parser in " << elapsed << ", " << N / elapsed
              << " parse/s\n";

    // Map case
    literal = "123_lbf";
    CHECK(dim::from_string(force, literal));
    start = std::chrono::system_clock::now();
    for (int i = 0; i < N; i++) { dim::from_string(force, literal); }
    stop = std::chrono::system_clock::now();
    elapsed = (stop - start) / std::chrono::nanoseconds(1) * 1e-9;
    std::cout << "Parsed " << N << " quantities from map in " << elapsed << ", " << N / elapsed << " parse/s\n";
}