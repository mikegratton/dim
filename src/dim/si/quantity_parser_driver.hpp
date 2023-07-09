#pragma once

#include "quantity.tab.hpp"
#include "si_io.hpp"

namespace dim {
namespace si {
namespace detail {

/**
 * Communication between bison parser and output
 */
class quantity_parser_driver {
   public:
    // Result of the parse
    ::dim::si::dynamic_quantity result;

    // Text being parsed
    char const* corpus;

    // Size of text (or -1 for null terminated text)
    int corpus_size;

    // Current location in the text
    char const* cursor;

    /**
     * @brief Parse the text into a dynamic_quantity, setting result by side-effect
     *
     * @param text Text to parse
     * @param text_size Size of text, or -1 for null-terminated text
     */
    int parse(char const* text, int text_size)
    {
        corpus = cursor = text;
        corpus_size = text_size;
        siquant::parser parse(*this);
        try {
            return parse();
        } catch (...) {
            result = ::dim::si::dynamic_quantity::bad_quantity();
            return -1;
        }
    }

    quantity_parser_driver() : result(::dim::si::dynamic_quantity::bad_quantity()) {}
};

}  // namespace detail
}  // namespace si
}  // namespace dim