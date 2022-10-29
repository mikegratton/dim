%define api.pure true
%define api.prefix {quantity}

%code top {
#include "dim/dynamic_quantity.hpp"
#include "dim/si/parse_unit.hpp"
}

%union {
  int integer;
  double scalar;
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
%token <integer> INTEGER
%token <scalar> SCALAR
%token <unit> UNIT

%type <quantity> output quantity unit_group
%type <integer> exponent_group

%left MULTIPLY DIVIDE
%right EXPONENT

%destructor { free($$); } <unit>

%%

output :
   quantity { *result = $$ = $1; return 0; }
   ;

quantity:
   SCALAR unit_group { $$ = $1 * $2; }
   | INTEGER unit_group { $$ = static_cast<double>($1) * $2; }
   | unit_group { $$ = $1; }
   ;
   
unit_group:   
   OPEN_PARENS unit_group CLOSE_PARENS { $$ = $2; }
   | unit_group EXPONENT exponent_group { $$ = power($1, $3); }
   | unit_group DIVIDE unit_group { $$ = divide($1, $3); }
   | unit_group MULTIPLY unit_group { $$ = multiply($1, $3); }   
   | MULTIPLY unit_group { $$ = $2; } 
   | UNIT { $$ = dim::si::detail::parse_known_quantity($1); }
   ;
   
 exponent_group:
    OPEN_PARENS exponent_group CLOSE_PARENS { $$ = $2; }
    | INTEGER 
    ;
    
 %%
 
 int quantityerror(void* yylval, ::dim::si::dynamic_quantity* val, void const* scanner)
 {
    /*
    QUANTITYSTYPE* lval = (QUANTITYSTYPE*) yylval;
    char* as_str = lval->unit;
    int as_int = lval->integer;
    printf("Parse error... %s, %d \n", as_str, as_int);
    */
    return 1;
 }
