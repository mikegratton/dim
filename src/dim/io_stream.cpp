#include "io_stream.hpp"

namespace dim {

namespace detail {

bool extract_quantity(char* o_buf, char*& o_unit, std::size_t bufmax, std::istream& is)
{
    std::size_t i = 0;
    while (is && std::isspace(is.peek())) { is.get(); }
    char_parse_state state = kStart;
    for (; i < bufmax && is; i++) {
        char c = is.get();
        if (is_float_part(c, state)) {
            o_buf[i] = c;
        } else if (c == '_' || std::isspace(c)) {
            o_buf[i] = '\0';
            break;
        } else {
            return false;
        }
    }
    if (state == kStart || state == kExponentStart) { return false; }
    if (i == bufmax) {
        o_buf[i - 1] = '\0';
        o_unit = &o_buf[i - 1];
        return false;
    }
    if (i + 1 == bufmax || state == kExponentStart) {
        o_unit = &o_buf[i];
        return false;
    }
    o_unit = &o_buf[i + 1];
    bufmax -= (i + 1);
    i = 0;
    state = kUnit;
    while (is && std::isspace(is.peek())) { is.get(); }
    for (; i < bufmax; i++) {
        char c = is.peek();
        if (is_unit_char(c, state)) {
            o_unit[i] = is.get();
        } else {
            break;
        }
    }
    if (i == bufmax || state == kExponentStart) {
        o_unit[bufmax - 1] = '\0';
        return false;
    }
    o_unit[i] = '\0';
    return true;
}
}  // namespace detail
}  // namespace dim
