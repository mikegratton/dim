#include "io.hpp"

namespace dim {
    
namespace detail {

char const* advance_past_separator(char const* unit_string)
{
    for (; *unit_string; ++unit_string) {    
        switch (*unit_string) {
            case ' ':
            case '*':
            case '_':                
                break;
            default:
                return unit_string;
        }
    }
    return unit_string;
}

bool is_unit_char(char c, char_parse_state& io_state)
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

// TODO hexfloat support
bool is_float_part(char c, char_parse_state& io_state)
{
    switch (io_state) {
        case kStart:
            if (c == '-' || c == '+') {
                io_state = kSignificand;
                return true;
            }
            if (c >= '0' && c <= '9') {
                io_state = kSignificand;
                return true;
            }
            return false;
        case kSignificand:
            if (c >= '0' && c <= '9') {
                io_state = kSignificand;
                return true;
            }
            if (c == 'e' || c == 'E') {
                io_state = kExponentStart;
                return true;
            }
            if (c == '.') {
                io_state = kFraction;
                return true;
            }
            return false;
        case kFraction:
            if (c >= '0' && c <= '9') {
                io_state = kFraction;
                return true;
            }
            if (c == 'e' || c == 'E') {
                io_state = kExponentStart;
                return true;
            }
            return false;
        case kExponentStart:
            if (c == '-' || c == '+') {
                io_state = kExponent;
                return true;
            }
            if (c >= '0' && c <= '9') {
                io_state = kExponent;
                return true;
            }
            return false;
        case kExponent:
            if (c >= '0' && c <= '9') {
                io_state = kExponent;
                return true;
            }
            return false;
        default: return false;
    }
}

}  // namespace detail
}  // namespace dim