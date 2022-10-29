#pragma once
#include <iostream>
#include <map>
#include <memory>
#include "io.hpp"
#include "dynamic_quantity.hpp"

namespace dim
{

class quantity_facet : public std::locale::facet
{
private:
    template <class Q, DIM_IS_QUANTITY(Q)> struct map_container : public detail::container_base {
        map_container() { }
        map_container(format_map<Q> const& map_) : map(map_) { }
        format_map<Q> map;
    };
    template<class S, class System>
    struct dynamic_map_container : public detail::container_base {
        dynamic_map_container() = default;
        dynamic_format_map<S, System> map;
    };
    std::map<long, std::unique_ptr<detail::container_base>> input_symbol;
    std::map<long, std::unique_ptr<detail::container_base>> output_symbol;
    std::map<long, std::unique_ptr<detail::container_base>> dynamic_output_symbol;
    std::map<long, std::unique_ptr<detail::container_base>> dynamic_input_symbol;

    quantity_facet(quantity_facet const& other) = delete;
    quantity_facet const& operator=(quantity_facet const& other) = delete;

public:
    static std::locale::id id;

    explicit quantity_facet(std::size_t refs = 0) : std::locale::facet(refs) { }

    quantity_facet(quantity_facet const* other, std::size_t refs) = delete;

    // These can't be deleted because the base class versions aren't deleted
    // quantity_facet(quantity_facet const& other) = delete;
    // quantity_facet const& operator=(quantity_facet const& other) = delete;

    /*
     * Format a quantity for output (convert to correct scalar value, assign symbol)
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    formatted_quantity<typename Q::scalar>
    format(Q const& q) const
    {
        auto it = output_symbol.find(formatter<Q>::index());
        if(it != output_symbol.end()) {
            return static_cast<formatter<Q> const*>(it->second.get())->output(q);
        }
        formatted_quantity<typename Q::scalar> fq(nullptr, q.value);
        bool spaceit = false;
        detail::print_unit<typename Q::unit>(fq.set_symbol(), typename Q::unit(), spaceit);
        return fq;
    }

    /*
     * Format a dynamic_quantity for output (convert to correct scalar value, assign symbol)
     */
    template<class S, class System>
    formatted_quantity<S>
    format(dynamic_quantity<S, System> const& q) const
    {
        auto it = dynamic_output_symbol.find(index(q.unit));
        if (it != dynamic_output_symbol.end()) {
            return static_cast<dynamic_formatter<S, System>>(it->second.get())->output(q);
        }
        formatted_quantity<S> fq(nullptr, q.value);
        bool spaceit = false;
        detail::print_unit<System>(fq.set_symbol(), q.unit, spaceit);
        return fq;
    }

    /*
     * Format a scalar + symbol into a quantity. If the conversion is illegal, the value
     * will be NaN (check with is_bad() method on quantity)
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    Q format(typename Q::scalar const& s, const char* symbol) const
    {
        auto it = input_symbol.find(formatter<Q>::index());
        if(it != input_symbol.end()) {
            // std::cout << "Found map for index " << formatter<Q>::index() << "\n";
            format_map<Q> const& map = static_cast<map_container<Q> const*>(it->second.get())->map;
            auto it = map.find(symbol);
            if(it != map.end()) {
                return it->second.input(s);
            }
        }
        // std::cout << "dynamic parse instead of map for " << formatter<Q>::index() << "\n";
        double scale;
        if(detail::parse_unit_dynamic<typename Q::unit>(scale, symbol)) {
            return Q(s*scale);
        }
        return Q::bad_quantity();
    }

    template<class S, class System>
    dynamic_quantity<S, System> format(S const& s, const char* symbol) const
    {
        auto it = input_symbol.find(System::id);
        if(it != input_symbol.end()) {
            auto const& map = static_cast<dynamic_map_container<S,System> const*>(it->second.get())->map;
            auto it = map.find(symbol);
            if(it != map.end()) {
                return it->second.input(s);
            }
        }        
        auto result = detail::parse_standard_rep<System, S>(symbol);
        result.value *= s;
        return result;        
    }
    
    /*
     * Attach a new output formatter for Q, replacing the existing formatter
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    void output_formatter(formatter<Q> const& f)
    {
        output_symbol.insert_or_assign(f.index(), std::make_unique<formatter<Q>>(f));
    }
    
    template<class S, class System>
    void output_formatter(dynamic_formatter<S, System> const& f)
    {
        dynamic_output_symbol.insert_or_assign(f.index(), std::make_unique<dynamic_formatter<S,System>>(f));
    }

    /*
     * Attach a new input formatter for Q, adding it to the map symbol->formatter
     * for this type, replacing any previous formatter with the same symbol for the
     * same type Q.
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    void input_formatter(formatter<Q> const& f)
    {
        format_map<Q>* map;
        auto it = input_symbol.find(formatter<Q>::index());
        if(it != input_symbol.end()) {
            map = &static_cast<map_container<Q>*>(it->second.get())->map;
        } else {
            auto idx = formatter<Q>::index();
            input_symbol.emplace(idx, std::make_unique<map_container<Q>>());
            map = &static_cast<map_container<Q>*>(input_symbol[idx].get())->map;
        }
        map->emplace(f.symbol, f);
    }

    /*
     * Replace the symbol->formatter map for Q with "map"
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    void input_formatter(format_map<Q> const& map)
    {
        input_symbol.emplace(formatter<Q>::index(), std::make_unique<map_container<Q>>(map));
    }

        /*
     * Attach a new input formatter for Q, adding it to the map symbol->formatter
     * for this type, replacing any previous formatter with the same symbol for the
     * same type Q.
     */
    template<class S, class System>
    void input_formatter(dynamic_formatter<S,System> const& f)
    {
        dynamic_format_map<S,System>* map;
        auto it = dynamic_input_symbol.find(System::id);
        if(it != dynamic_input_symbol.end()) {
            map = &static_cast<dynamic_map_container<S,System>*>(it->second.get())->map;
        } else {
            auto idx = f.index();
            dynamic_input_symbol.emplace(idx, std::make_unique<dynamic_map_container<S,System>>());
            map = &static_cast<dynamic_map_container<S,System>*>(dynamic_input_symbol[idx].get())->map;
        }
        map->emplace(f.symbol, f);
    }

    /*
     * Replace the symbol->formatter map for Q with "map"
     */
    template<class S, class System>
    void input_formatter(dynamic_format_map<S,System> const& map)
    {        
        dynamic_input_symbol[System::id] = std::make_unique<dynamic_map_container<S,System>>(map);        
    }
    
    /*
     * Drop input formatters for Q (reverting to default format)
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    void clear_input_formatter()
    {
        auto it = input_symbol.find(formatter<Q>::index());
        if(it != input_symbol.end()) {
            input_symbol.erase(it);
        }
    }

    /*
     * Drop all output formatters for Q (reverting to default format)
     */
    template<class Q, DIM_IS_QUANTITY(Q)>
    void clear_output_formatter()
    {
        auto it = output_symbol.find(formatter<Q>::index());
        if(it != output_symbol.end()) {
            output_symbol.erase(it);
        }
    }

    /*
     * Drop ALL input formatters
     */
    void clear_input_formatters()
    {
        input_symbol.clear();
    }

    /*
     * Drop ALL output formatters
     */
    void clear_output_formatters()
    {
        output_symbol.clear();
    }
    
    void clear_dynamic_output_formatters()
    {
        dynamic_output_symbol.clear();
    }
    
    void clear_dynamic_input_formatters()
    {
        dynamic_input_symbol.clear();
    }

    /*
     * Drop ALL formatters for input and output
     */
    void clear()
    {
        clear_input_formatters();
        clear_output_formatters();
        clear_dynamic_output_formatters();
        clear_dynamic_input_formatters();
    }
};

template<class scalar>
inline std::ostream& operator<< (std::ostream& os, formatted_quantity<scalar> const& fq)
{
    return os << fq.value() << "_" << fq.symbol();
}


template<class Q, DIM_IS_QUANTITY(Q)>
std::ostream& operator<< (std::ostream& os, Q const& q)
{
    if(std::has_facet<quantity_facet> (os.getloc())) {
        os << std::use_facet<quantity_facet> (os.getloc()).format<Q>(q);
    } else {
        os << q.value << "_" << typename Q::unit();
    }
    return os;
}


template<class S, class System, DIM_IS_SCALAR(S)>
std::ostream& operator<< (std::ostream& os, dynamic_quantity<S, System> const& dq)
{
    if(std::has_facet<quantity_facet> (os.getloc())) {
        os << std::use_facet<quantity_facet> (os.getloc()).format(dq);
    } else {
        char buf[128];
        print_unit<System>(buf, dq.unit, true);
        os << dq.value << buf;
    }
    return os;
}


template<class U, DIM_IS_UNIT(U)>
std::ostream& operator<< (std::ostream& os, unit_base<U> const& u)
{
    bool spaceit = false;
    char buf[128];
    detail::print_unit<U>(buf, u, spaceit);
    os << buf;
    return os;
}

/*
 * Calls parse_quantity, using the quantity_facet to extract a custom unit map if available.
 */
template<class Q, DIM_IS_QUANTITY(Q)>
std::istream& operator>>(std::istream& is, Q& out_q)
{
    typename Q::scalar value;
    char unit_string[128];
    is >> value >> std::skipws >> unit_string;
    char* unit_start = unit_string[0] == '_' ? unit_string + 1 : unit_string;
    for (char* cursor = unit_start; *cursor != 0; cursor++) {
        if (*cursor == ',' ) {
            *cursor = '\0';
            is.putback(',');
            break;
        }
    }
    if(std::has_facet<quantity_facet>(is.getloc())) {
        out_q = std::use_facet<quantity_facet>(is.getloc()).format<Q>(value, unit_start);
        // std::cout << "Using facet for " << unit_start;
        if(out_q.is_bad()) {
            is.setstate(std::ios_base::failbit);
        }
    } else if(!parse_quantity<Q> (out_q, value, unit_start)) {
        // std::cout << "Parse failed for " << unit_start;
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

template<class S, class System>
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
    if(std::has_facet<quantity_facet> (is.getloc())) {
        o_q = std::use_facet<quantity_facet> (is.getloc()).format<S,System>(value, unit_start);
        if (o_q.is_bad()) {
            is.setstate(std::ios_base::failbit);
        }
        return is;
    }         
    o_q = detail::parse_standard_rep<System, S>(unit_string);
    o_q.value *= value;
    if (o_q.is_bad()) {
        is.setstate(std::ios_base::failbit);
    }
    return is;
}

} // end of namespace dim

