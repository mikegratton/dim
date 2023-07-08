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

"/" { return '/'; }

"^" { return '^'; }

"(" { return '('; }

")" { return ')'; }

[a-zA-Z] { return yytext[0]; }

. { return BAD_INPUT; }

%%
