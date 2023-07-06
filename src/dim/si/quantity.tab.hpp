/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

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

/* Token kinds.  */
#ifndef QUANTITYTOKENTYPE
# define QUANTITYTOKENTYPE
  enum quantitytokentype
  {
    QUANTITYEMPTY = -2,
    QUANTITYEOF = 0,               /* "end of file"  */
    QUANTITYerror = 256,           /* error  */
    QUANTITYUNDEF = 257,           /* "invalid token"  */
    MULTIPLY = 258,                /* MULTIPLY  */
    DIVIDE = 259,                  /* DIVIDE  */
    EXPONENT = 260,                /* EXPONENT  */
    OPEN_PARENS = 261,             /* OPEN_PARENS  */
    CLOSE_PARENS = 262,            /* CLOSE_PARENS  */
    INTEGER = 263,                 /* INTEGER  */
    UNIT = 264,                    /* UNIT  */
    BAD_INPUT = 265                /* BAD_INPUT  */
  };
  typedef enum quantitytokentype quantitytoken_kind_t;
#endif

/* Value type.  */
#if ! defined QUANTITYSTYPE && ! defined QUANTITYSTYPE_IS_DECLARED
union QUANTITYSTYPE
{
#line 9 "quantity.y"

  int integer;  
  char unit[8];
  ::dim::si::dynamic_quantity quantity;

#line 88 "../quantity.tab.hpp"

};
typedef union QUANTITYSTYPE QUANTITYSTYPE;
# define QUANTITYSTYPE_IS_TRIVIAL 1
# define QUANTITYSTYPE_IS_DECLARED 1
#endif




int quantityparse (void* scanner, ::dim::si::dynamic_quantity* result);


#endif /* !YY_QUANTITY_QUANTITY_TAB_HPP_INCLUDED  */
