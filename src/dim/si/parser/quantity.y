%define api.pure true
%define api.prefix {quantity}

%code top {
#include "dim/dynamic_quantity.hpp"
#include "dim/si/parse_unit.hpp"
}

%union {
  int integer;  
  double scale;
  ::dim::si::dynamic_quantity quantity; 
}


%param { void* scanner }

%parse-param {
    ::dim::si::dynamic_quantity* result
}


%code {
    int quantityerror(void* lval, ::dim::si::dynamic_quantity* val, void const* scanner);
    int quantitylex(YYSTYPE* lval, void* scanner);
}

%token MULTIPLY
%token <integer> INTEGER
%token BAD_INPUT
%type <integer> exponent_group
%type <quantity> unit_group unit unit_literal
%type <scale> prefix

%left MULTIPLY '/'
%right '^'

%destructor { } <*>

%%

output :
   unit_group             { *result = $1; return 0; }   
   | unit_group BAD_INPUT { *result = ::dim::si::dynamic_quantity::bad_quantity(); return 1; }
   | BAD_INPUT unit_group { *result = ::dim::si::dynamic_quantity::bad_quantity(); return 1; }
   | BAD_INPUT            { *result = ::dim::si::dynamic_quantity::bad_quantity(); return 1; }
   | error                { *result = ::dim::si::dynamic_quantity::bad_quantity(); return 1; }
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
 
int quantityerror(void* yylval, ::dim::si::dynamic_quantity* val, void const* scanner)
{
   *val = ::dim::si::dynamic_quantity::bad_quantity();
   return 1;
}
