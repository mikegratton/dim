%define api.pure true
%define api.prefix {quantity}

%code top {
#include "dim/dynamic_quantity.hpp"
#include "dim/si/parse_unit.hpp"
}

%union {
  int integer;  
  char* unit;
  ::dim::si::dynamic_quantity quantity;
}


%param { void* scanner }

%parse-param {::dim::si::dynamic_quantity* result}


%code {
    int quantityerror(void* lval, ::dim::si::dynamic_quantity* val, void const* scanner);
    int quantitylex(YYSTYPE* lval, void* scanner);
}

%token MULTIPLY
%token DIVIDE
%token EXPONENT
%token OPEN_PARENS
%token CLOSE_PARENS
%token BAD_INPUT
%token <integer> INTEGER
%token <unit> UNIT
%type <integer> exponent_group
%type <quantity> unit_group
%type <quantity> output

%left MULTIPLY DIVIDE
%right EXPONENT

%destructor { free($$); } <unit>

%%

output :
   unit_group { *result = $$ = $1; return 0; }
   | error { return 1; }
   ;
   
unit_group:   
   OPEN_PARENS unit_group CLOSE_PARENS { $$ = $2; }
   | unit_group EXPONENT exponent_group { $$ = power($1, $3); }
   | unit_group DIVIDE unit_group { $$ = divide($1, $3); }
   | unit_group MULTIPLY unit_group { $$ = multiply($1, $3); }      
   | UNIT { $$ = dim::si::detail::parse_known_quantity($1); }      
   | unit_group BAD_INPUT { return 1; }
   ;
   
exponent_group:
   OPEN_PARENS exponent_group CLOSE_PARENS { $$ = $2; }
   | INTEGER         
   | exponent_group BAD_INPUT { return 1; }
   ;
    
%%
 
int quantityerror(void* yylval, ::dim::si::dynamic_quantity* val, void const* scanner)
{
   return 1;
}
