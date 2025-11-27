#include "io_detail.hpp"
#include <cassert>
#if __cplusplus >= 201703L
#include <charconv>
#endif

namespace dim {
    
namespace detail {

bool is_separator_character(char c)
{
    switch (c) {
        case '.': 
        case '_':
        case '*':
        case ' ':
            return true;
        default:
            return false;
        }
}

char const* advance_past_separator(char const* symbol, char const* end)
{
    for (; symbol < end && is_separator_character(*symbol); ++symbol) {  }
    return symbol;
}

bool is_unit_char(char c, unit_parse_state& io_state)
{
    switch (io_state) {
        case kUnit:
            if (c >= 'A' && c <= 'Z') { return true; }
            if (c >= 'a' && c <= 'z') { return true; }
            if (c == '*' || c == '/' || c == '_') { return true; }
            if (c == '^') {
                io_state = kExponent;
                return true;
            }
            return false;
        case kExponent:
            if (c >= '0' && c <= '9') { return true; }
            if (c >= 'A' && c <= 'Z') {
                io_state = kUnit;
                return true;
            }
            if (c >= 'a' && c <= 'z') {
                io_state = kUnit;
                return true;
            }
            if (c == '*' || c == '/' || c == '_') {
                io_state = kUnit;
                return true;
            }
            return false;
        default: return false;
    }
}


// requires at most 4 chars of space, no null terminator
#if __cplusplus >= 201703L
char* print_int8(char* o_buf, int8_t d)
{
    auto result = std::to_chars(o_buf, o_buf+4, d);
    assert(result.ec == std::errc{});
    return result.ptr;
}
#else
char* print_int8(char* o_buf, int8_t d)
{
    // first, deal with tje sign
    unsigned int big_d;
    if (d < 0) {
        *o_buf++ = '-';
        big_d = static_cast<unsigned int>(-static_cast<int>(d));
    } else {
        big_d = static_cast<unsigned int>(d);
    }

    // Determine the number of digits
    unsigned int scale = 100;
    unsigned int digits = 3;
    while (big_d / scale == 0 && scale > 1) {
        scale /= 10;
        digits--;
    }

    for (unsigned int i = 0; i < digits; i++) {
        unsigned int digit = big_d / scale;
        big_d -= digit * scale;
        scale /= 10;
        *o_buf++ = static_cast<char>(digit) + '0';
    }
    return o_buf;
}
#endif

char* print_exponentiated_symbol(char* out_symbol, char const* end, int8_t exponent, char const* symbol, bool& spaceit)
{
    // "_in^5" -- Spacer, symbol, caret, exponent    
    if (exponent == 0 || out_symbol == end) { return out_symbol; }
    if (spaceit) { *out_symbol++ = '_'; }     
    while (*symbol && out_symbol < end) { *out_symbol++ = *symbol++; }
    if (out_symbol < end && exponent != 1) {
        *out_symbol++ = '^'; 
        out_symbol = print_int8(out_symbol, exponent);
    }
    spaceit = true;
    return out_symbol;
}

}  // namespace detail
}  // namespace dim