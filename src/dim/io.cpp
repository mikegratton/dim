#include "io_detail.hpp"
#include <cassert>
#include <cctype>
#include <memory>
#if __cplusplus >= 201703L
#include <charconv>
#endif

namespace dim
{

namespace detail
{

static bool isoperator(char c) { return (c == '*' || c == '/' || c == '_'); }

bool unit_string_scanner::issymbol(char c)
{
    unsigned char u = static_cast<unsigned char>(c);
    if (m_utf_prev == 0) {
        if (isalpha(c)) {
            return true;
        }
        switch (u) {            
            case 0xc2 : // Degree mark            
            case 0xce : // Greek 1
            case 0xcf : // Greek 2
            case 0xe2 : // Misc extra byte 1            
                m_utf_prev = u;
                return true;
            default:
                return false;
        }        
    } else {
        // utf-8 second char        
        if (m_utf_prev == 0xc2 && u == 0xb0) { // Degree
            m_utf_prev = 0;
            return true;        
        } else if (m_utf_prev == 0xce && (u >= 0x91 && u <= 0xbf)) { // Greek 1
            m_utf_prev = 0;
            return true;
        } else if (m_utf_prev == 0xcf && (u >= 0x80 && u <= 0xbf)) { // Greek 2
            m_utf_prev = 0;
            return true;
        } else if (m_utf_prev == 0xe2 && u == 0x84) { // Misc 2nd byte
            m_utf_prev = 0x84;
            return true;
        } else if (m_utf_prev == 0x84 && (u >= 0x83 && u <= 0xab )) { // Misc 3rd byte
            m_utf_prev = 0;
            return true;
        }
    }
    return false;
}

bool unit_string_scanner::accept(char c)
{
    switch (m_state) {
    case kSymbol:
        m_state = handle_symbol(c);
        break;
    case kOperator:
        if (issymbol(c)) {
            m_state = kSymbol;
        } else if (c == '(') {
            m_parenthesis_depth += 1;
        } else {
            m_state = kError;
        }
        break;
    case kExponentStart:
        m_state = handle_exponent_start(c);
        break;
    case kExponentSign:
        m_state = (isdigit(c) ? kExponent : kError);
        break;
    case kExponent:
        m_state = handle_exponent(c);
        break;
    case kError:
        break;
    case kEnd:
    default:
        m_state = kEnd;
    }
    return wrap_up();
}

bool unit_string_scanner::wrap_up()
{
    if (m_state == kEnd && (m_parenthesis_depth > 0 || m_exponent_parenthesis || m_utf_prev != 0)) {
        m_state = kError;
    }
    return !(m_state == kError || m_state == kEnd);
}

unit_parse_state unit_string_scanner::handle_symbol(char c)
{
    if (issymbol(c)) {
        return kSymbol;
    } else if (isoperator(c)) {
        return kOperator;
    } else if (c == ')') {
        m_parenthesis_depth -= 1;
        return (m_parenthesis_depth >= 0 ? kSymbol : kError);
    } else if (c == '^') {
        return kExponentStart;
    }
    return kEnd;
}

unit_parse_state unit_string_scanner::handle_exponent_start(char c)
{
    if (c == '-' || c == '+') {
        return kExponentSign;
    } else if (c == '(') {
        if (m_exponent_parenthesis) {
            return kError;
        } else {
            m_exponent_parenthesis = true;
            return kExponentStart;
        }
    } else if (isdigit(c)) {
        return kExponent;
    }
    return kError;
}

unit_parse_state unit_string_scanner::handle_exponent(char c)
{
    if (isdigit(c)) {
        return kExponent;
    } else if (isoperator(c)) {
        return kOperator;
    } else if (c == ')') {
        if (m_exponent_parenthesis) {
            m_exponent_parenthesis = false;
            return kSymbol;
        } else if (m_parenthesis_depth > 0) {
            m_parenthesis_depth -= 1;
            return kSymbol;
        } else {
            return kError;
        }
    }
    return kEnd;
}

// requires at most 4 chars of space, no null terminator
#if __cplusplus >= 201703L
char* print_int8(char* o_buf, int8_t d)
{
    auto result = std::to_chars(o_buf, o_buf + 4, d);
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
    if (exponent == 0 || out_symbol == end) {
        return out_symbol;
    }
    if (spaceit) {
        *out_symbol++ = '_';
    }
    while (*symbol && out_symbol < end) {
        *out_symbol++ = *symbol++;
    }
    if (out_symbol < end && exponent != 1) {
        *out_symbol++ = '^';
        out_symbol = print_int8(out_symbol, exponent);
    }
    spaceit = true;
    return out_symbol;
}

} // namespace detail
} // namespace dim
