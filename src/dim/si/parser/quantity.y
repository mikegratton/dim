%language "c++"
%require "3.8" 
%skeleton "lalr1.cc" 

%header
%define api.prefix {siquant}
%define api.value.type variant

%code requires {
    #include "dim/dynamic_quantity.hpp"
    #include "dim/si/si_io.hpp"
    namespace dim {
    namespace si {
    namespace detail {
    class quantity_parser_driver;
    }
    }
    }
}

%param { ::dim::si::detail::quantity_parser_driver& driver }


%code {
    #include "quantity_parser_driver.hpp"

    int yylex( siquant::parser::value_type* o_value, ::dim::si::detail::quantity_parser_driver& io_driver);

}

%token BAD_INTEGER
%token MULTIPLY
%token <int> INTEGER
%type <int> exponent_group
%type <::dim::si::dynamic_quantity> unit_group unit unit_literal
%type <double> prefix

%left MULTIPLY '/'
%right '^'

%%

output :
   %empty
   | unit_group           { driver.result = $1; return 0; }  
   | MULTIPLY unit_group  { driver.result = $2; return 0; }    
   | error                { driver.result = ::dim::si::dynamic_quantity::bad_quantity(); return 1; }
   ;
   
unit_group:   
   '(' unit_group ')'                 { $$ = $2;               }
   | unit_group '^' exponent_group    { $$ = power($1, $3);    }
   | unit_group '/' unit_group        { $$ = divide($1, $3);   }
   | unit_group MULTIPLY unit_group   { $$ = multiply($1, $3); }
   | unit   
   ;
   
unit :
   prefix unit_literal { $$ = $1 * $2; }
   | unit_literal 
   ;

prefix :
      'y' { $$ = 1e-24; }
    | 'z' { $$ = 1e-21; }
    | 'a' { $$ = 1e-18; }
    | 'f' { $$ = 1e-15; }
    | 'p' { $$ = 1e-12; }
    | 'n' { $$ = 1e-9; }
    | 'u' { $$ = 1e-6; }
    | 'm' { $$ = 1e-3; }
    | 'c' { $$ = 1e-2; }
    | 'd' { $$ = 1e-1; }
    | 'Y' { $$ = 1e24; }
    | 'Z' { $$ = 1e21; }
    | 'E' { $$ = 1e18; }
    | 'P' { $$ = 1e15; }
    | 'T' { $$ = 1e12; }
    | 'G' { $$ = 1e9; }
    | 'M' { $$ = 1e6; }
    | 'k' { $$ = 1e3; }
    | 'h' { $$ = 1e2; }
    ;

unit_literal :
      'm'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 1,  0,  0,  0,  0,  0,  0,  0}); }
    | 's'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  1,  0,  0,  0,  0,  0,  0}); }
    | 'g'         { $$ = ::dim::si::dynamic_quantity(1e-3, { 0,  0,  1,  0,  0,  0,  0,  0}); }
    | 'r' 'a' 'd' { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  1,  0,  0,  0,  0}); }
    | 'K'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  1,  0,  0,  0}); }
    | 'm' 'o' 'l' { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  0,  1,  0,  0}); }
    | 'A'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  0,  0,  1,  0}); }
    | 'c' 'd'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  0,  0,  0,  1}); }
    | 'H' 'z'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 0, -1,  0,  0,  0,  0,  0,  0}); }
    | 's' 'r'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  2,  0,  0,  0,  0}); }
    | 'N'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 1, -2,  1,  0,  0,  0,  0,  0}); } 
    | 'P' 'a'     { $$ = ::dim::si::dynamic_quantity(1e0,  {-1, -2,  1,  0,  0,  0,  0,  0}); }
    | 'J'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  1,  0,  0,  0,  0,  0}); }
    | 'W'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -3,  1,  0,  0,  0,  0,  0}); }
    | 'C'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  1,  0,  0,  0,  0,  1,  0}); }
    | 'V'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -3,  1,  0,  0,  0, -1,  0}); }
    | 'F'         { $$ = ::dim::si::dynamic_quantity(1e0,  {-2,  4, -1,  0,  0,  0,  2,  0}); }
    | 'R'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -3,  1,  0,  0,  0, -2,  0}); }
    | 'S'         { $$ = ::dim::si::dynamic_quantity(1e0,  {-2,  3, -1,  0,  0,  0,  2,  0}); }
    | 'W' 'b'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  1,  0,  0,  0, -1,  0}); }
    | 'T'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 0, -2,  1,  0,  0,  0, -1,  0}); }
    | 'H'         { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  1,  0,  0,  0, -2,  0}); }
    | 'I' 'm'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  2,  0,  0,  0,  1}); }
    | 'I' 'x'     { $$ = ::dim::si::dynamic_quantity(1e0,  {-2,  0,  0,  2,  0,  0,  0,  1}); }
    | 'B' 'q'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 0, -1,  0,  0,  0,  0,  0,  0}); }
    | 'G' 'y'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  0,  0,  0,  0,  0,  0}); }
    | 'S' 'v'     { $$ = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  0,  0,  0,  0,  0,  0}); }
    | 'k' 'a' 't' { $$ = ::dim::si::dynamic_quantity(1e0,  { 0, -1,  0,  0,  0,  1,  0,  0}); }
    | 'L'         { $$ = ::dim::si::dynamic_quantity(1e-3, { 3,  0,  0,  0,  0,  0,  0,  0}); }    
    | 'b' 'a' 'r' { $$ = ::dim::si::dynamic_quantity(1e5,  {-1, -2,  1,  0,  0,  0,  0,  0}); }
    | 'e' 'V'     { $$ = ::dim::si::dynamic_quantity(1.60218e-19, {2, -2, 1, 0, 0, 0, 0, 0}); }    
    ;

exponent_group:
   '(' exponent_group ')' { $$ = $2; }
   | INTEGER        
   ;
 
%%

int yylex(siquant::parser::value_type* o_typePtr, ::dim::si::detail::quantity_parser_driver& io_driver)
{
    if (io_driver.cursor >= io_driver.corpus_end) {
        return siquant::parser::token::SIQUANTEOF;
    }
    unsigned char c = static_cast<unsigned char>(*io_driver.cursor++);
    switch(c) {
        case '.':
        case '*':
        case '_':
        case ' ':
            return siquant::parser::token::MULTIPLY;
        case '-':
        case '+':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            char* endPtr;
            o_typePtr->emplace<int>(strtol(io_driver.cursor-1, &endPtr, 10));
            if (endPtr == io_driver.cursor-1) {
                return siquant::parser::token::BAD_INTEGER;
            } else {
                io_driver.cursor = endPtr;
                return siquant::parser::token::INTEGER;
            }
        }
        case 0xce : {
            unsigned char d = static_cast<unsigned char>(*io_driver.cursor++);
            switch(d) {
                case 0xa9 :
                    return 'R';
                case 0xbc :
                    return 'u';
                default:
                    return siquant::parser::token::BAD_INTEGER;
            }
        }
        case 0xe2 : {
            unsigned char d = static_cast<unsigned char>(*io_driver.cursor++);
            if (d != 0x84) {
                return siquant::parser::token::BAD_INTEGER;
            }
            d = static_cast<unsigned char>(*io_driver.cursor++);            
            switch(d) {
                case 0xaa :
                    return 'K';
                case 0xa6 :
                    return 'R';
                default:
                    return siquant::parser::token::BAD_INTEGER;
            }
        }
        default:
            return c;
    }
}

void siquant::parser::error(std::string const&) 
{
    this->driver.result = ::dim::si::dynamic_quantity::bad_quantity();    
}