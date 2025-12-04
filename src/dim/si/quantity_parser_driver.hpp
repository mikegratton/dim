#pragma once

#include "definition.hpp"
#include "dim/si/si_facet.hpp"
#include "quantity.tab.hpp"

namespace dim
{
namespace si
{
namespace detail
{

/**
 * Communication between bison parser and output. Use parse_standard_rep() to
 * access this functionality.
 *
 * @note The public members are referenced by name in the bison-generated code.  
 */
class quantity_parser_driver
{
  public:
    /// Result of the parse
    ::dim::si::dynamic_quantity result;

    /// Text being parsed
    char const* corpus;

    /// Current location in the text
    char const* cursor;

    /// Pointer past the end of the text
    char const* corpus_end;

    /**
     * @brief Parse the text into a dynamic_quantity, setting result by side-effect
     *
     * @param text Text to parse
     * @param text_size Size of text, or -1 for null-terminated text
     */
    int parse(char const* text, char const* text_end)
    {
        result = si::dynamic_quantity(1.0, si::dynamic_unit::dimensionless());
        corpus = cursor = text;
        corpus_end = text_end;
        siquant::parser bison_parser(*this);
        try {
            return bison_parser();
        } catch (...) {
            result = ::dim::si::dynamic_quantity::bad_quantity();
            return -1;
        }
    }

    /**
     * Construct a parser_driver in a non-functional state
     */ 
    quantity_parser_driver()
        : result(::dim::si::dynamic_quantity::bad_quantity()),
          corpus(nullptr),
          cursor(nullptr),
          corpus_end(nullptr)
    {
    }
};

} // namespace detail
} // namespace si
} // namespace dim