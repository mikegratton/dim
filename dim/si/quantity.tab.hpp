/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_QUANTITY_QUANTITY_TAB_HPP_INCLUDED
# define YY_QUANTITY_QUANTITY_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef QUANTITYDEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define QUANTITYDEBUG 1
#  else
#   define QUANTITYDEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define QUANTITYDEBUG 0
# endif /* ! defined YYDEBUG */
#endif  /* ! defined QUANTITYDEBUG */
#if QUANTITYDEBUG
extern int quantitydebug;
#endif

/* Token type.  */
#ifndef QUANTITYTOKENTYPE
# define QUANTITYTOKENTYPE
  enum quantitytokentype
  {
    MULTIPLY = 258,
    DIVIDE = 259,
    EXPONENT = 260,
    OPEN_PARENS = 261,
    CLOSE_PARENS = 262,
    INTEGER = 263,
    SCALAR = 264,
    UNIT = 265
  };
#endif

/* Value type.  */
#if ! defined QUANTITYSTYPE && ! defined QUANTITYSTYPE_IS_DECLARED
union QUANTITYSTYPE
{
#line 9 "quantity.y"

  int integer;
  double scalar;
  char* unit;
  ::dim::si::dynamic_quantity quantity;

#line 83 "../quantity.tab.hpp"

};
typedef union QUANTITYSTYPE QUANTITYSTYPE;
# define QUANTITYSTYPE_IS_TRIVIAL 1
# define QUANTITYSTYPE_IS_DECLARED 1
#endif



int quantityparse (void* scanner, ::dim::si::dynamic_quantity* result);

#endif /* !YY_QUANTITY_QUANTITY_TAB_HPP_INCLUDED  */
