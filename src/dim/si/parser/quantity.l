%option bison-bridge reentrant
%option noyywrap
%option prefix="quantity"

%{

#include "dim/si/parse_unit.hpp"
#include "dim/si/si_io.hpp"
#include "quantity.tab.hpp"
#include <cstdlib>
#include <cstring>

#define YYSTYPE QUANTITYSTYPE

%}

%% 

[-+]?[0-9]+ { yylval->integer = atoi(yytext); return INTEGER; }

"*" { return MULTIPLY; }

"_" { return MULTIPLY; }

" " { return MULTIPLY; }

"/" { return DIVIDE; }

"^" { return EXPONENT; }

"(" { return OPEN_PARENS; }

")" { return CLOSE_PARENS; }

[a-zA-Z]+ { strncpy(yylval->unit, yytext, sizeof(yylval->unit)); return UNIT; }

. { return BAD_INPUT; }

%%
