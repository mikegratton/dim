#pragma once
#include <iostream>
#include <memory>
#include <string>

#include "dim/base.hpp"
#include "dynamic_quantity.hpp"
#include "format_map.hpp"
#include "io.hpp"

#if __cplusplus < 201402L
namespace std {
template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace std
#endif

namespace dim {

template <class Map>
std::unique_ptr<Map> make_unique_map()
{
    return std::unique_ptr<Map>(new Map());
}

template <class Map>
std::unique_ptr<Map> make_unique_map(Map const& f)
{
    std::unique_ptr<Map> mapPtr = make_unique_map<Map>();
    *mapPtr = f;
    return mapPtr;
}

/// An iostream facet for formatting quantities. This is typically used by operator<< and operator>>,
/// but can be adjusted to change how quantities are formated in a program
class quantity_facet : public std::locale::facet {
   private:
    format_index_map input_symbol;
    format_index_map output_symbol;
    format_index_map dynamic_input_symbol;
    format_index_map dynamic_output_symbol;

    quantity_facet(quantity_facet const& other) = delete;
    quantity_facet const& operator=(quantity_facet const& other) = delete;

   public:
    static std::locale::id id;

    explicit quantity_facet(std::size_t refs = 0) : std::locale::facet(refs) {}

    quantity_facet(quantity_facet const* other, std::size_t refs) = delete;

    // These can't be deleted because the base class versions aren't deleted
    // quantity_facet(quantity_facet const& other) = delete;
    // quantity_facet const& operator=(quantity_facet const& other) = delete;

    /**
     * @brief Format a quantity for output (convert to correct scalar value, assign symbol)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    formatted_quantity<typename Q::scalar> format(Q const& q) const
    {
        auto const* base = output_symbol.get(Q::index());
        if (base) { return static_cast<formatter<Q> const*>(base)->output(q); }
        formatted_quantity<typename Q::scalar> fq(nullptr, dimensionless_cast(q));
        bool spaceit = false;
        detail::print_unit<typename Q::unit>(fq.set_symbol(), typename Q::unit(), spaceit);
        return fq;
    }

    /**
     * @brief Format a dynamic_quantity for output (convert to correct scalar value, assign symbol)
     */
    template <class S, class System>
    formatted_quantity<S> format(dynamic_quantity<S, System> const& q) const
    {
        auto const* base = dynamic_output_symbol.get(index(q.unit));
        if (base) { return static_cast<dynamic_formatter<S, System>>(base)->output(q); }
        formatted_quantity<S> fq(nullptr, dimensionless_cast(q));
        bool spaceit = false;
        detail::print_unit<System>(fq.set_symbol(), q.unit, spaceit);
        return fq;
    }

    /**
     * @brief Format a scalar + symbol into a quantity. If the conversion is illegal, the value
     * will be NaN (check with is_bad() method on quantity)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    Q format(typename Q::scalar const& s, const char* symbol) const
    {
        auto const* base = input_symbol.get(Q::index());
        if (base) {
            auto const* formatter = static_cast<format_map<Q> const*>(base)->get(symbol);
            if (formatter) { return formatter->input(s); }
        }
        // std::cout << "dynamic parse instead of map for " << formatter<Q>::index() << "\n";
        double scale;
        if (detail::parse_unit_dynamic<typename Q::unit>(scale, symbol)) { return Q(s * scale); }
        return Q::bad_quantity();
    }

    /**
     * @brief Format a scalar and a symbol to a dynamic_quantity
     */
    template <class S, class System>
    dynamic_quantity<S, System> format(S const& s, const char* symbol) const
    {
        auto const* base = dynamic_input_symbol.get(System::id);
        if (base) {
            auto const* map = static_cast<dynamic_formatter<S, System> const*>(base);
            auto const* format = map.get(symbol);
            if (format) { return format->input(s); }
        }
        auto result = detail::parse_standard_rep<System, S>(symbol);
        dimensionless_cast(result) *= s;
        return result;
    }

    /**
     * @brief Attach a new output formatter for Q, replacing the existing formatter
     *
     * @param f The new output formatter for quantity Q
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void output_formatter(formatter<Q> const& f)
    {
        output_symbol.set(f.index(), std::make_unique<formatter<Q>>(f));
    }

    /**
     * @brief Attach a new output formatter for Q, replacing the existing formatter
     *
     * @param symbol Symbol for format
     * @param scale  One unit of symbol is this much of Q
     * @param add    (For affine maps, defaults to zero)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void output_formatter(char const* symbol, Q scale, Q add = Q(0.0))
    {
        output_formatter(formatter<Q>(symbol, scale, add));
    }

    /**
     * @brief Attach a new output formatter for dynamic quantities (all of them)
     * @param f The new output formatter.
     */
    template <class S, class System>
    void output_formatter(dynamic_formatter<S, System> const& f)
    {
        dynamic_output_symbol.set(f.index(), make_unique_map<dynamic_formatter<S, System>>(f));
    }

    /**
     * @brief Attach a new input formatter for Q, adding it to the map symbol->formatter
     * for this type, replacing any previous formatter with the same symbol for the
     * same type Q.
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void input_formatter(formatter<Q> const& f)
    {
        format_map<Q>* map = static_cast<format_map<Q>*>(input_symbol.get(f.index()));
        if (map) {
            map->set(f);
        } else {
            auto new_map = make_unique_map<format_map<Q>>();
            new_map->set(f);
            input_symbol.set(f.index(), std::move(new_map));
        }
    }

    /**
     * @brief Replace the symbol->formatter map for Q with "map"
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void input_formatter(format_map<Q> const& map)
    {
        input_symbol.set(Q::index(), make_unique_map<format_map<Q>>(map));
    }

    /**
     * @brief Attach a new input formatter for Q, adding it to the map symbol->formatter
     * for this type, replacing any previous formatter with the same symbol for the
     * same type Q.
     */
    template <class S, class System>
    void input_formatter(dynamic_formatter<S, System> const& f)
    {
        dynamic_format_map<S, System>* map =
            static_cast<dynamic_format_map<S, System>*>(dynamic_input_symbol.get(f.index()));
        if (map) {
            map->set(f);
        } else {
            auto new_map = make_unique_map<dynamic_format_map<S, System>>();
            new_map->set(f);
            dynamic_input_symbol.set(f.index(), std::move(new_map));
        }
    }

    /**
     * @brief Replace the symbol->formatter map for Q with "map"
     */
    template <class S, class System>
    void input_formatter(dynamic_format_map<S, System> const& map)
    {
        dynamic_input_symbol.set(System::id, make_unique_map<dynamic_format_map<S, System>>(map));
    }

    /**
     * @brief Drop input formatters for Q (reverting to default format)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void clear_input_formatter()
    {
        input_symbol.erase(Q::index());
    }

    /**
     * @brief Drop all output formatters for Q (reverting to default format)
     */
    template <class Q, DIM_IS_QUANTITY(Q)>
    void clear_output_formatter()
    {
        output_symbol.erase(Q::index());
    }

    /**
     * @brief Drop ALL input formatters
     */
    void clear_input_formatters() { input_symbol.clear(); }

    /**
     * @brief Drop ALL output formatters
     */
    void clear_output_formatters() { output_symbol.clear(); }

    /// Clear dynamic_quantity output formatters
    void clear_dynamic_output_formatters() { dynamic_output_symbol.clear(); }

    /// Clear dynamic_quantity input formatters
    void clear_dynamic_input_formatters() { dynamic_input_symbol.clear(); }

    /**
     * @brief Drop ALL formatters for input and output
     */
    void clear()
    {
        clear_input_formatters();
        clear_output_formatters();
        clear_dynamic_output_formatters();
        clear_dynamic_input_formatters();
    }
};

/// Write a formatted quantity to a stream
template <class scalar>
inline std::ostream& operator<<(std::ostream& os, formatted_quantity<scalar> const& fq)
{
    return os << fq.value() << "_" << fq.symbol();
}

/// Write a quantity Q to a stream using the facet
template <class Q, DIM_IS_QUANTITY(Q)>
std::ostream& operator<<(std::ostream& os, Q const& q)
{
    if (std::has_facet<quantity_facet>(os.getloc())) {
        os << std::use_facet<quantity_facet>(os.getloc()).format<Q>(q);
    } else {
        os << dimensionless_cast(q) << "_" << typename Q::unit();
    }
    return os;
}

/// Write a dynamic_quantity to a stream using the facet
template <class S, class System, DIM_IS_SCALAR(S)>
std::ostream& operator<<(std::ostream& os, dynamic_quantity<S, System> const& dq)
{
    if (std::has_facet<quantity_facet>(os.getloc())) {
        os << std::use_facet<quantity_facet>(os.getloc()).format(dq);
    } else {
        char buf[128];
        print_unit<System>(buf, dq.unit, true);
        os << dq.value << buf;
    }
    return os;
}

/// Write the unit U to a stream using the default output symbol
template <class U, DIM_IS_UNIT(U)>
std::ostream& operator<<(std::ostream& os, unit_base<U> const& u)
{
    bool spaceit = false;
    char buf[128];
    detail::print_unit<U>(buf, u, spaceit);
    os << buf;
    return os;
}

/**
 * @breif Calls parse_quantity, using the quantity_facet to extract a custom unit map if available.
 */
template <class Q, DIM_IS_QUANTITY(Q)>
std::istream& operator>>(std::istream& is, Q& out_q)
{
    typename Q::scalar value;
    char unit_string[128];
    is >> value >> std::skipws >> unit_string;
    char* unit_start = unit_string[0] == '_' ? unit_string + 1 : unit_string;
    for (char* cursor = unit_start; *cursor != 0; cursor++) {
        if (*cursor == ',') {
            *cursor = '\0';
            is.putback(',');
            break;
        }
    }
    if (std::has_facet<quantity_facet>(is.getloc())) {
        out_q = std::use_facet<quantity_facet>(is.getloc()).format<Q>(value, unit_start);
        if (out_q.is_bad()) { is.setstate(std::ios_base::failbit); }
    } else if (!parse_quantity<Q>(out_q, value, unit_start)) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

/// Use the facet to do formatted input for a dynamic quantity
template <class S, class System>
std::istream& operator>>(std::istream& is, dynamic_quantity<S, system_tag>& o_q)
{
    S value;
    char unit_string[128];
    is >> value >> std::skipws >> unit_string;
    char* unit_start = unit_string[0] == '_' ? unit_string + 1 : unit_string;
    for (char* cursor = unit_start; *cursor != 0; cursor++) {
        if (*cursor == ',') {
            *cursor = '\0';
            is.putback(',');
            break;
        }
    }
    if (std::has_facet<quantity_facet>(is.getloc())) {
        o_q = std::use_facet<quantity_facet>(is.getloc()).format<S, System>(value, unit_start);
        if (o_q.is_bad()) { is.setstate(std::ios_base::failbit); }
        return is;
    }
    o_q = detail::parse_standard_rep<System, S>(unit_string);
    o_q.value *= value;
    if (o_q.is_bad()) { is.setstate(std::ios_base::failbit); }
    return is;
}

/// Make a string representation of Q using the facet
template <class Q, DIM_IS_QUANTITY(Q)>
std::string to_string(Q i_quantity)
{
    std::locale loc;  // Get the global locale
    if (std::has_facet<quantity_facet>(loc)) {
        auto formatted = std::use_facet<quantity_facet>(loc).format(i_quantity);
        return std::to_string(formatted.value()) + '_' + formatted.symbol();
    }
    char raw[64];
    print_quantity(raw, i_quantity);
    return std::string(raw);
}

/// Parse a string representation to Q using the facet. If the string
/// does not represent a quantity of type Q, return false
template <class Q, DIM_IS_QUANTITY(Q)>
bool from_string(Q& o_quantity, std::string const& i_string)
{
    typename Q::scalar value;
    std::size_t endOfNumber;
    value = std::stod(i_string, &endOfNumber);
    if (i_string[endOfNumber] == '_') { endOfNumber++; }
    std::string unit_string = i_string.substr(endOfNumber, i_string.find(","));

    std::locale loc;  // Get the global locale
    if (std::has_facet<quantity_facet>(loc)) {
        o_quantity = std::use_facet<quantity_facet>(loc).format<Q>(value, unit_string.c_str());
        if (o_quantity.is_bad()) { return false; }
    } else if (!parse_quantity<Q>(o_quantity, value, unit_string.c_str())) {
        return false;
    }
    return true;
}

}  // end of namespace dim
