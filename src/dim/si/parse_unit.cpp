#include "parse_unit.hpp"
#include "definition.hpp"
#include <map>

namespace dim {
namespace si {
namespace detail {

constexpr double PREFIX_NOT_FOUND = 0.0;
    
const std::pair<char, double> si_prefix[] {
    {'y',  1e-24},
    {'z',  1e-21},
    {'a',  1e-18},
    {'f',  1e-15},
    {'p',  1e-12},
    {'n',  1e-9},
    {'u',  1e-6},
    {'m',  1e-3},
    {'c',  1e-2},
    {'d',  1e-1},
    {'Y',  1e24},
    {'Z',  1e21},
    {'E',  1e18},
    {'P',  1e15},
    {'T',  1e12},
    {'G',  1e9},
    {'M',  1e6},
    {'k',  1e3},
    {'h',  1e2},
};

const std::pair<const char*, std::pair<double, dynamic_unit>> si_symbol[] {
    {"m",   {1e0,  { 1, 0, 0,  0, 0,  0, 0, 0}}},
    {"s",   {1e0,  { 0, 1, 0,  0, 0,  0, 0, 0}}},
    {"g",   {1e-3, { 0, 0, 1,  0, 0,  0, 0, 0}}},
    {"rad", {1e0,  { 0, 0, 0,  1, 0,  0, 0, 0}}},
    {"K",   {1e0,  { 0, 0, 0,  0, 1,  0, 0, 0}}},
    {"mol", {1e0,  { 0, 0, 0,  0, 0,  1, 0, 0}}},
    {"A",   {1e0,  { 0, 0, 0,  0, 0,  0, 1, 0}}},
    {"cd",  {1e0,  { 0, 0, 0,  0, 0,  0, 0, 1}}},
    // Derived units with special symbols (NIST list)
    {"Hz",  {1e0,  { 0, -1, 0,  0, 0,  0, 0, 0}}},
    {"sr",  {1e0,  { 0, 0, 0,  2, 0,  0, 0, 0}}},
    {"N",   {1e0,  { 1, -2, 1,  0, 0,  0, 0, 0}}},
    {"Pa",  {1e0,  {-1, -2, 1,  0, 0,  0, 0, 0}}},
    {"J",   {1e0,  { 2, -2, 1,  0, 0,  0, 0, 0}}},
    {"W",   {1e0,  { 2, -3, 1,  0, 0,  0, 0, 0}}},
    {"C",   {1e0,  { 0, 1, 0,  0, 0,  0, 1, 0}}},
    {"V",   {1e0,  { 2, -3, 1,  0, 0,  0, -1, 0}}},
    {"F",   {1e0,  {-2, 4, -1,  0, 0,  0, 2, 0}}},
    {"R",   {1e0,  { 2, -3, 1,  0, 0,  0, -2, 0}}},
    {"S",   {1e0,  {-2, 3, -1,  0, 0,  0, 2, 0}}},
    {"Wb",  {1e0,  { 2, -2, 1,  0, 0,  0, -1, 0}}},
    {"T",   {1e0,  { 0, -2, 1,  0, 0,  0, -1, 0}}},
    {"H",   {1e0,  { 2, -2, 1,  0, 0,  0, -2, 0}}},
    {"Im",  {1e0,  { 0, 0, 0,  2, 0,  0, 0, 1}}},
    {"Ix",  {1e0,  {-2, 0, 0,  2, 0,  0, 0, 1}}},
    {"Bq",  {1e0,  { 0, -1, 0,  0, 0,  0, 0, 0}}},
    {"Gy",  {1e0,  { 2, -2, 0,  0, 0,  0, 0, 0}}},
    {"Sv",  {1e0,  { 2, -2, 0,  0, 0,  0, 0, 0}}},
    {"kat", {1e0,  { 0, -1, 0,  0, 0,  1, 0, 0}}},
    // Outside units accepted for SI use
    {"L",   {1e-3, { 3, 0, 0,  0, 0,  0, 0, 0}}},
    {"eV",  {1.60218e-19, { 2, -2, 1,  0, 0,  0, 0, 0}}},
    // Other units provisionally accepted
    {"a",   {1e2,  { 2, 0, 0,  0, 0,  0, 0, 0}}}, // This is a problem as it conflicts with atto
    {"bar", {1e5,  {-1, -2, 1,  0, 0,  0, 0, 0}}}
};

double find_si_prefix(int& start, const char* buf)
{
    static bool s_INIT = false;
    static std::map<char, double> SI_PREFIX; 
    if(!s_INIT) {
        s_INIT = true;
        int n = sizeof(si_prefix) / sizeof(std::pair<char, double>);
        for(int i = 0; i < n; i++) {
            SI_PREFIX.emplace(si_prefix[i]);
        }
    }
    start = 0;
    double val = PREFIX_NOT_FOUND;
    if(buf[0] == 'd' && buf[1] == 'a') {
        val = 1e1;
        start = 2;
        return val;
    }
    auto it = SI_PREFIX.find(buf[0]);
    if(it != SI_PREFIX.end()) {
        val = it->second;
        start = 1;
    }
    return val;
}

dynamic_quantity parse_known_quantity(const char* buf)
{
    static bool s_INIT=false;
    static std::map<const char*, dynamic_quantity, ::dim::detail::char_less> s_SYMBOL;
    if(!s_INIT) {  // Store data in compact form
        int n = sizeof(si_symbol)/sizeof(std::pair<const char*, std::pair<double, dynamic_unit>>);
        for(int i=0; i<n; i++) {
            auto const& d = si_symbol[i];
            s_SYMBOL.emplace(d.first,  dynamic_quantity{d.second.first, d.second.second});
        }
        s_INIT = true;
    }
    auto it = s_SYMBOL.find(buf);
    if(it != s_SYMBOL.end()) {
        return it->second;
    } else {
        // Try again with a prefix 
        int start = 0;
        double prefix = find_si_prefix(start, buf);
        if (prefix == PREFIX_NOT_FOUND) {
            return dynamic_quantity::bad_quantity();
        }
        return prefix * parse_known_quantity(buf + start);                
    }    
}


}
}
}
