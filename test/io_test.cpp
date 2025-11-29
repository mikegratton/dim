#include <cstring>
#include <sstream>
#include "dim/io_detail.hpp"
#include "dim/ioformat.hpp"
#include "dim/si.hpp"
#include "doctest.h"

TEST_CASE("from_chars")
{
    // No number
    si::formatted_quantity fq(1.0, "m");
    char const* test = "ff";
    auto result = dim::from_chars(test, test + strlen(test), fq);
    CHECK_MESSAGE(result.ec != std::errc{}, "Error code: ", std::make_error_code(result.ec).message());
    CHECK(result.ptr == test);
    CHECK(fq.is_bad());
    CHECK(fq.symbol()[0] == '\0');

    // No unit
    test = "1";
    result = dim::from_chars(test, test + strlen(test), fq);
    CHECK_MESSAGE(result.ec == std::errc{}, "Error code: ", std::make_error_code(result.ec).message());
    CHECK(fq.value() == 1.0);
    CHECK(std::string(fq.symbol()) == "");

    // Usual
    test = "123.456E-11_m^-1*kg";
    result = dim::from_chars(test, test + strlen(test), fq);
    CHECK_MESSAGE(result.ec == std::errc{}, "Error code: ", std::make_error_code(result.ec).message(), ", value = ", fq.value(), ", symbol = ", std::string(fq.symbol()));
    CHECK(result.ptr == test + strlen(test));
    CHECK(fq.value() == doctest::Approx(123.456e-11));
    CHECK(std::string(fq.symbol()) == "m^-1*kg");

    // Non-unit end char
    test = "-123.456e+11_m^-1*kg[";
    result = dim::from_chars(test, test + strlen(test), fq);
    CHECK_MESSAGE(result.ec == std::errc{}, "Error code: ", std::make_error_code(result.ec).message());
    CHECK_MESSAGE(*result.ptr == '[', "Expected [, got ", std::string(result.ptr));
    CHECK(fq.value() == doctest::Approx(-123.456e+11));
    CHECK(std::string(fq.symbol()) == "m^-1*kg");

    // Whitespce
    test = "123.456E-11_m ^-1*kg";
    result = dim::from_chars(test, test + strlen(test), fq);
    CHECK_MESSAGE(result.ec == std::errc{}, "Error code: ", std::make_error_code(result.ec).message());
    CHECK_MESSAGE(*result.ptr == ' ', "Expected space, got ", std::string(result.ptr));
    CHECK(fq.value() == doctest::Approx(123.456e-11));
    CHECK(std::string(fq.symbol()) == "m");
    
}

TEST_CASE("print_unit")
{
    char buf[1024];
    char* cursor = print_unit(buf, buf + 1024, si::dynamic_unit(si::meter_));
    CHECK(std::string(buf) == "m");
    CHECK(cursor == buf+1);

    cursor = print_unit(buf, buf + 1024, si::meter);
    CHECK(std::string(buf) == "m");
    CHECK(cursor == buf+1);

    cursor = print_unit(buf, buf + 1024, si::dynamic_quantity(si::meter));
    CHECK(std::string(buf) == "m");
    CHECK(cursor == buf+1);

    cursor = print_unit(buf, buf + 1024, si::dynamic_quantity(si::meter).unit());
    CHECK(std::string(buf) == "m");
    CHECK(cursor == buf+1);

    si::dynamic_unit u(1,2,3,4,5,6,7,-8);
    cursor = print_unit(buf, buf + 1024, u);
    CHECK(std::string(buf) == "rad^4_kg^3_m_K^5_mol^6_A^7_cd^-8_s^2");

    u.angle(0);
    u.amount(0);
    cursor = print_unit(buf, buf + 1024, u);
    CHECK(std::string(buf) == "kg^3_m_K^5_A^7_cd^-8_s^2");

}

TEST_CASE("print_int8")
{
    char buf[8];
    memset(buf, 0, 8);
    int8_t d = -128;
    char* cursor = dim::detail::print_int8(buf, d);
    CHECK(cursor - buf == 4);
    CHECK(std::string(buf) == "-128");
    
    d = 0;
    memset(buf, 0, 8);
    cursor = dim::detail::print_int8(buf, d);
    CHECK(std::string(buf) == "0");
    CHECK(cursor - buf == 1);

    d = 127;
    memset(buf, 0, 8);
    cursor = dim::detail::print_int8(buf, d);
    CHECK(std::string(buf) == "127");
    CHECK(cursor - buf == 3);

    d = 12;
    memset(buf, 0, 8);
    cursor = dim::detail::print_int8(buf, d);
    CHECK(std::string(buf) == "12");
    CHECK(cursor - buf == 2);
}

TEST_CASE("isseparator")
{
    CHECK(dim::detail::isseparator('.'));
    CHECK(dim::detail::isseparator('*'));
    CHECK(dim::detail::isseparator('_'));
    CHECK(dim::detail::isseparator(' '));
    for (char c = 'a'; c <= 'z'; c++) {
        CHECK_FALSE(dim::detail::isseparator(c));
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        CHECK_FALSE(dim::detail::isseparator(c));
    }
    for (char c = '0'; c <= '9'; c++) {
        CHECK_FALSE(dim::detail::isseparator(c));
    }
}

TEST_CASE("unit_string_scanner.basic")
{
    char const* basic = "kg^-123*m^2/s_mol";
    char const* cursor = basic;
    dim::detail::unit_string_scanner scanner;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponentStart);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponentSign);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponent);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponent);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponent);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kOperator);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponentStart);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kExponent);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kOperator);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kOperator);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);
}


TEST_CASE("unit_string_scanner.exponent_error")
{
    char const* test = "s^--123";
    dim::detail::unit_string_scanner scanner;
    char const* cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);

    test = "s^^2";
    scanner.reset();
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);

    test = "s^*";
    scanner.reset();
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);

    test = "s^m";
    scanner.reset();
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);
}

TEST_CASE("unit_string_scanner.operator_error")
{
    char const* test = "s*/m";
    dim::detail::unit_string_scanner scanner;
    char const* cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));    
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);

    test = "s*^1";
    scanner.reset();
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.accept(*cursor++));    
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);
}

TEST_CASE("unit_string_scanner.paren")
{
    char const* test = "(m*kg)/s^(-2)";
    dim::detail::unit_string_scanner scanner;
    char const* cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kEnd);

    scanner.reset();
    test = "((m))";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kEnd);

    scanner.reset();
    test = "(m^-1)";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kEnd);

    scanner.reset();
    test = "(m^(-1))";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kEnd);
    
    scanner.reset();
    test = "(m/s)";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kEnd);

    scanner.reset();
    test = "(m";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kError);

    scanner.reset();
    test = "((m)";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kError);

    scanner.reset();
    test = "(m^(-1)";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kError);

    scanner.reset();
    test = "m^(-1";
    cursor = test;
    while (scanner.accept(*cursor++)) { }
    CHECK(scanner.state() == dim::detail::kError);
}

TEST_CASE("unit_string_scanner.utf8")
{
    // mu
    char const* test = "μm/s";
    dim::detail::unit_string_scanner scanner;
    char const* cursor = test;    
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);    
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kOperator);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Omega
    scanner.reset();
    test = "Ωm";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Degree sign
    scanner.reset();
    test = "°/s";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kOperator);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Angstrom
    scanner.reset();
    test = "Å";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Celsius
    scanner.reset();
    test = "℃";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Fahrenheit
    scanner.reset();
    test = "℉";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Start of greek range
    scanner.reset();
    test = "Α";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // End of greek range
    scanner.reset();
    test = "Ͽ";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kEnd);

    // Before greek range
    scanner.reset();
    test = "ΐ";
    cursor = test;
    CHECK(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kSymbol);    
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);

    // After greek range
    scanner.reset();
    test = "Ѐ";
    cursor = test;
    CHECK_FALSE(scanner.accept(*cursor++));
    CHECK(scanner.state() == dim::detail::kError);
}

