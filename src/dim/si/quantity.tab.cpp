// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.


// Take the name prefix into account.
#define yylex   siquantlex



#include "quantity.tab.hpp"


// Unqualified %code blocks.
#line 24 "quantity.y"

    #include "quantity_parser_driver.hpp"

    int yylex( siquant::parser::value_type* o_value, ::dim::si::detail::quantity_parser_driver& io_driver);


#line 55 "../quantity.tab.cpp"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if SIQUANTDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !SIQUANTDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !SIQUANTDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace siquant {
#line 128 "../quantity.tab.cpp"

  /// Build a parser object.
  parser::parser (::dim::si::detail::quantity_parser_driver& driver_yyarg)
#if SIQUANTDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  parser::~parser ()
  {}

  parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
  {
    switch (this->kind ())
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        value.copy< ::dim::si::dynamic_quantity > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_prefix: // prefix
        value.copy< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        value.copy< int > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  parser::symbol_kind_type
  parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        value.move< ::dim::si::dynamic_quantity > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_prefix: // prefix
        value.move< double > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        value.move< int > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

  }

  // by_kind.
  parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  parser::symbol_kind_type
  parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  parser::symbol_kind_type
  parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  parser::symbol_kind_type
  parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  parser::stack_symbol_type::stack_symbol_type ()
  {}

  parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        value.YY_MOVE_OR_COPY< ::dim::si::dynamic_quantity > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_prefix: // prefix
        value.YY_MOVE_OR_COPY< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        value.YY_MOVE_OR_COPY< int > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s)
  {
    switch (that.kind ())
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        value.move< ::dim::si::dynamic_quantity > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_prefix: // prefix
        value.move< double > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        value.move< int > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        value.copy< ::dim::si::dynamic_quantity > (that.value);
        break;

      case symbol_kind::S_prefix: // prefix
        value.copy< double > (that.value);
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        value.copy< int > (that.value);
        break;

      default:
        break;
    }

    return *this;
  }

  parser::stack_symbol_type&
  parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        value.move< ::dim::si::dynamic_quantity > (that.value);
        break;

      case symbol_kind::S_prefix: // prefix
        value.move< double > (that.value);
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        value.move< int > (that.value);
        break;

      default:
        break;
    }

    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if SIQUANTDEBUG
  template <typename Base>
  void
  parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if SIQUANTDEBUG
  std::ostream&
  parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  parser::debug_level_type
  parser::debug_level () const
  {
    return yydebug_;
  }

  void
  parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // SIQUANTDEBUG

  parser::state_type
  parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  parser::operator() ()
  {
    return parse ();
  }

  int
  parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, driver));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_unit_group: // unit_group
      case symbol_kind::S_unit: // unit
      case symbol_kind::S_unit_literal: // unit_literal
        yylhs.value.emplace< ::dim::si::dynamic_quantity > ();
        break;

      case symbol_kind::S_prefix: // prefix
        yylhs.value.emplace< double > ();
        break;

      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_exponent_group: // exponent_group
        yylhs.value.emplace< int > ();
        break;

      default:
        break;
    }



      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 3: // output: unit_group
#line 45 "quantity.y"
                          { driver.result = yystack_[0].value.as < ::dim::si::dynamic_quantity > (); return 0; }
#line 708 "../quantity.tab.cpp"
    break;

  case 4: // output: MULTIPLY unit_group
#line 46 "quantity.y"
                          { driver.result = yystack_[0].value.as < ::dim::si::dynamic_quantity > (); return 0; }
#line 714 "../quantity.tab.cpp"
    break;

  case 5: // output: error
#line 47 "quantity.y"
                          { driver.result = ::dim::si::dynamic_quantity::bad_quantity(); return 1; }
#line 720 "../quantity.tab.cpp"
    break;

  case 6: // unit_group: '(' unit_group ')'
#line 51 "quantity.y"
                                      { yylhs.value.as < ::dim::si::dynamic_quantity > () = yystack_[1].value.as < ::dim::si::dynamic_quantity > ();               }
#line 726 "../quantity.tab.cpp"
    break;

  case 7: // unit_group: unit_group '^' exponent_group
#line 52 "quantity.y"
                                      { yylhs.value.as < ::dim::si::dynamic_quantity > () = power(yystack_[2].value.as < ::dim::si::dynamic_quantity > (), yystack_[0].value.as < int > ());    }
#line 732 "../quantity.tab.cpp"
    break;

  case 8: // unit_group: unit_group '/' unit_group
#line 53 "quantity.y"
                                      { yylhs.value.as < ::dim::si::dynamic_quantity > () = divide(yystack_[2].value.as < ::dim::si::dynamic_quantity > (), yystack_[0].value.as < ::dim::si::dynamic_quantity > ());   }
#line 738 "../quantity.tab.cpp"
    break;

  case 9: // unit_group: unit_group MULTIPLY unit_group
#line 54 "quantity.y"
                                      { yylhs.value.as < ::dim::si::dynamic_quantity > () = multiply(yystack_[2].value.as < ::dim::si::dynamic_quantity > (), yystack_[0].value.as < ::dim::si::dynamic_quantity > ()); }
#line 744 "../quantity.tab.cpp"
    break;

  case 10: // unit_group: unit
#line 55 "quantity.y"
     { yylhs.value.as < ::dim::si::dynamic_quantity > () = yystack_[0].value.as < ::dim::si::dynamic_quantity > (); }
#line 750 "../quantity.tab.cpp"
    break;

  case 11: // unit: prefix unit_literal
#line 59 "quantity.y"
                       { yylhs.value.as < ::dim::si::dynamic_quantity > () = yystack_[1].value.as < double > () * yystack_[0].value.as < ::dim::si::dynamic_quantity > (); }
#line 756 "../quantity.tab.cpp"
    break;

  case 12: // unit: unit_literal
#line 60 "quantity.y"
     { yylhs.value.as < ::dim::si::dynamic_quantity > () = yystack_[0].value.as < ::dim::si::dynamic_quantity > (); }
#line 762 "../quantity.tab.cpp"
    break;

  case 13: // prefix: 'y'
#line 64 "quantity.y"
          { yylhs.value.as < double > () = 1e-24; }
#line 768 "../quantity.tab.cpp"
    break;

  case 14: // prefix: 'z'
#line 65 "quantity.y"
          { yylhs.value.as < double > () = 1e-21; }
#line 774 "../quantity.tab.cpp"
    break;

  case 15: // prefix: 'a'
#line 66 "quantity.y"
          { yylhs.value.as < double > () = 1e-18; }
#line 780 "../quantity.tab.cpp"
    break;

  case 16: // prefix: 'f'
#line 67 "quantity.y"
          { yylhs.value.as < double > () = 1e-15; }
#line 786 "../quantity.tab.cpp"
    break;

  case 17: // prefix: 'p'
#line 68 "quantity.y"
          { yylhs.value.as < double > () = 1e-12; }
#line 792 "../quantity.tab.cpp"
    break;

  case 18: // prefix: 'n'
#line 69 "quantity.y"
          { yylhs.value.as < double > () = 1e-9; }
#line 798 "../quantity.tab.cpp"
    break;

  case 19: // prefix: 'u'
#line 70 "quantity.y"
          { yylhs.value.as < double > () = 1e-6; }
#line 804 "../quantity.tab.cpp"
    break;

  case 20: // prefix: 'm'
#line 71 "quantity.y"
          { yylhs.value.as < double > () = 1e-3; }
#line 810 "../quantity.tab.cpp"
    break;

  case 21: // prefix: 'c'
#line 72 "quantity.y"
          { yylhs.value.as < double > () = 1e-2; }
#line 816 "../quantity.tab.cpp"
    break;

  case 22: // prefix: 'd'
#line 73 "quantity.y"
          { yylhs.value.as < double > () = 1e-1; }
#line 822 "../quantity.tab.cpp"
    break;

  case 23: // prefix: 'Y'
#line 74 "quantity.y"
          { yylhs.value.as < double > () = 1e24; }
#line 828 "../quantity.tab.cpp"
    break;

  case 24: // prefix: 'Z'
#line 75 "quantity.y"
          { yylhs.value.as < double > () = 1e21; }
#line 834 "../quantity.tab.cpp"
    break;

  case 25: // prefix: 'E'
#line 76 "quantity.y"
          { yylhs.value.as < double > () = 1e18; }
#line 840 "../quantity.tab.cpp"
    break;

  case 26: // prefix: 'P'
#line 77 "quantity.y"
          { yylhs.value.as < double > () = 1e15; }
#line 846 "../quantity.tab.cpp"
    break;

  case 27: // prefix: 'T'
#line 78 "quantity.y"
          { yylhs.value.as < double > () = 1e12; }
#line 852 "../quantity.tab.cpp"
    break;

  case 28: // prefix: 'G'
#line 79 "quantity.y"
          { yylhs.value.as < double > () = 1e9; }
#line 858 "../quantity.tab.cpp"
    break;

  case 29: // prefix: 'M'
#line 80 "quantity.y"
          { yylhs.value.as < double > () = 1e6; }
#line 864 "../quantity.tab.cpp"
    break;

  case 30: // prefix: 'k'
#line 81 "quantity.y"
          { yylhs.value.as < double > () = 1e3; }
#line 870 "../quantity.tab.cpp"
    break;

  case 31: // prefix: 'h'
#line 82 "quantity.y"
          { yylhs.value.as < double > () = 1e2; }
#line 876 "../quantity.tab.cpp"
    break;

  case 32: // unit_literal: 'm'
#line 86 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 1,  0,  0,  0,  0,  0,  0,  0}); }
#line 882 "../quantity.tab.cpp"
    break;

  case 33: // unit_literal: 's'
#line 87 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  1,  0,  0,  0,  0,  0,  0}); }
#line 888 "../quantity.tab.cpp"
    break;

  case 34: // unit_literal: 'g'
#line 88 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e-3, { 0,  0,  1,  0,  0,  0,  0,  0}); }
#line 894 "../quantity.tab.cpp"
    break;

  case 35: // unit_literal: 'r' 'a' 'd'
#line 89 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  1,  0,  0,  0,  0}); }
#line 900 "../quantity.tab.cpp"
    break;

  case 36: // unit_literal: 'K'
#line 90 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  1,  0,  0,  0}); }
#line 906 "../quantity.tab.cpp"
    break;

  case 37: // unit_literal: 'm' 'o' 'l'
#line 91 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  0,  1,  0,  0}); }
#line 912 "../quantity.tab.cpp"
    break;

  case 38: // unit_literal: 'A'
#line 92 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  0,  0,  1,  0}); }
#line 918 "../quantity.tab.cpp"
    break;

  case 39: // unit_literal: 'c' 'd'
#line 93 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  0,  0,  0,  0,  1}); }
#line 924 "../quantity.tab.cpp"
    break;

  case 40: // unit_literal: 'H' 'z'
#line 94 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0, -1,  0,  0,  0,  0,  0,  0}); }
#line 930 "../quantity.tab.cpp"
    break;

  case 41: // unit_literal: 's' 'r'
#line 95 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  2,  0,  0,  0,  0}); }
#line 936 "../quantity.tab.cpp"
    break;

  case 42: // unit_literal: 'N'
#line 96 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 1, -2,  1,  0,  0,  0,  0,  0}); }
#line 942 "../quantity.tab.cpp"
    break;

  case 43: // unit_literal: 'P' 'a'
#line 97 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  {-1, -2,  1,  0,  0,  0,  0,  0}); }
#line 948 "../quantity.tab.cpp"
    break;

  case 44: // unit_literal: 'J'
#line 98 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  1,  0,  0,  0,  0,  0}); }
#line 954 "../quantity.tab.cpp"
    break;

  case 45: // unit_literal: 'W'
#line 99 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -3,  1,  0,  0,  0,  0,  0}); }
#line 960 "../quantity.tab.cpp"
    break;

  case 46: // unit_literal: 'C'
#line 100 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  1,  0,  0,  0,  0,  1,  0}); }
#line 966 "../quantity.tab.cpp"
    break;

  case 47: // unit_literal: 'V'
#line 101 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -3,  1,  0,  0,  0, -1,  0}); }
#line 972 "../quantity.tab.cpp"
    break;

  case 48: // unit_literal: 'F'
#line 102 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  {-2,  4, -1,  0,  0,  0,  2,  0}); }
#line 978 "../quantity.tab.cpp"
    break;

  case 49: // unit_literal: 'R'
#line 103 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -3,  1,  0,  0,  0, -2,  0}); }
#line 984 "../quantity.tab.cpp"
    break;

  case 50: // unit_literal: 'S'
#line 104 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  {-2,  3, -1,  0,  0,  0,  2,  0}); }
#line 990 "../quantity.tab.cpp"
    break;

  case 51: // unit_literal: 'W' 'b'
#line 105 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  1,  0,  0,  0, -1,  0}); }
#line 996 "../quantity.tab.cpp"
    break;

  case 52: // unit_literal: 'T'
#line 106 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0, -2,  1,  0,  0,  0, -1,  0}); }
#line 1002 "../quantity.tab.cpp"
    break;

  case 53: // unit_literal: 'H'
#line 107 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  1,  0,  0,  0, -2,  0}); }
#line 1008 "../quantity.tab.cpp"
    break;

  case 54: // unit_literal: 'I' 'm'
#line 108 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0,  0,  0,  2,  0,  0,  0,  1}); }
#line 1014 "../quantity.tab.cpp"
    break;

  case 55: // unit_literal: 'I' 'x'
#line 109 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  {-2,  0,  0,  2,  0,  0,  0,  1}); }
#line 1020 "../quantity.tab.cpp"
    break;

  case 56: // unit_literal: 'B' 'q'
#line 110 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0, -1,  0,  0,  0,  0,  0,  0}); }
#line 1026 "../quantity.tab.cpp"
    break;

  case 57: // unit_literal: 'G' 'y'
#line 111 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  0,  0,  0,  0,  0,  0}); }
#line 1032 "../quantity.tab.cpp"
    break;

  case 58: // unit_literal: 'S' 'v'
#line 112 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 2, -2,  0,  0,  0,  0,  0,  0}); }
#line 1038 "../quantity.tab.cpp"
    break;

  case 59: // unit_literal: 'k' 'a' 't'
#line 113 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e0,  { 0, -1,  0,  0,  0,  1,  0,  0}); }
#line 1044 "../quantity.tab.cpp"
    break;

  case 60: // unit_literal: 'L'
#line 114 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e-3, { 3,  0,  0,  0,  0,  0,  0,  0}); }
#line 1050 "../quantity.tab.cpp"
    break;

  case 61: // unit_literal: 'b' 'a' 'r'
#line 115 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1e5,  {-1, -2,  1,  0,  0,  0,  0,  0}); }
#line 1056 "../quantity.tab.cpp"
    break;

  case 62: // unit_literal: 'e' 'V'
#line 116 "quantity.y"
                  { yylhs.value.as < ::dim::si::dynamic_quantity > () = ::dim::si::dynamic_quantity(1.60218e-19, {2, -2, 1, 0, 0, 0, 0, 0}); }
#line 1062 "../quantity.tab.cpp"
    break;

  case 63: // exponent_group: '(' exponent_group ')'
#line 120 "quantity.y"
                          { yylhs.value.as < int > () = yystack_[1].value.as < int > (); }
#line 1068 "../quantity.tab.cpp"
    break;

  case 64: // exponent_group: INTEGER
#line 121 "quantity.y"
     { yylhs.value.as < int > () = yystack_[0].value.as < int > (); }
#line 1074 "../quantity.tab.cpp"
    break;


#line 1078 "../quantity.tab.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  parser::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if SIQUANTDEBUG || 0
  const char *
  parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if SIQUANTDEBUG || 0









  const signed char parser::yypact_ninf_ = -13;

  const signed char parser::yytable_ninf_ = -53;

  const short
  parser::yypact_[] =
  {
       2,   -13,    57,    57,   -13,   -13,   -13,   -13,   -13,   -13,
     -13,   139,   -11,   -13,   -13,   -13,   -13,    -3,    52,     1,
     -13,    24,   -13,    18,   -13,    39,   -13,   -13,    42,   -13,
     -13,    12,   -13,   -13,   -13,   -13,    10,    50,   -12,    17,
     -13,    49,    91,   147,   -13,    89,   -13,   147,   143,    70,
     -13,   -13,   -13,    60,   -13,    96,   -13,   -13,   -13,    77,
     -13,   -13,   -13,   -13,   -13,    57,    57,    -1,    84,   -11,
      -3,   -13,     1,    24,   -13,   -13,   -13,   -13,   -13,   -13,
     115,   115,   -13,    -1,   -13,   114,   -13
  };

  const signed char
  parser::yydefact_[] =
  {
       0,     5,     0,     0,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    33,    34,     0,    36,    38,    53,    42,
      44,    45,    46,    47,    48,    49,    50,     0,     0,     0,
      60,     0,     0,     3,    10,     0,    12,     4,     0,     0,
      39,    43,    57,     0,    41,     0,    40,    51,    58,     0,
      54,    55,    56,    62,     1,     0,     0,     0,    32,     0,
       0,    52,     0,     0,    11,     6,    37,    59,    35,    61,
       9,     8,    64,     0,     7,     0,    63
  };

  const signed char
  parser::yypgoto_[] =
  {
     -13,   -13,    -2,   -13,   -13,    93,    53
  };

  const signed char
  parser::yydefgoto_[] =
  {
       0,    42,    43,    44,    45,    46,    84
  };

  const signed char
  parser::yytable_[] =
  {
      47,    48,    -2,     1,    82,    60,     2,    83,    50,    51,
       3,    52,     4,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    61,    53,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    54,
      39,    55,   -52,    56,    40,    41,   -52,    57,   -52,   -52,
      58,   -52,    59,    80,    81,     3,    62,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      63,    64,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    76,    39,    68,    69,    79,    40,
      41,    77,    70,    71,    72,    78,    73,    49,    23,    24,
      25,    26,    67,    86,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    85,    39,    74,   -32,
       0,    40,    41,   -32,     0,   -32,   -32,    65,   -32,    66,
      67,    65,    75,    66,    67,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    49
  };

  const signed char
  parser::yycheck_[] =
  {
       2,     3,     0,     1,     5,    17,     4,     8,    19,    12,
       8,    10,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
      28,    29,    30,    31,    32,    47,    12,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    31,
      48,    12,     0,    11,    52,    53,     4,    45,     6,     7,
      50,     9,    12,    65,    66,     8,    49,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      41,     0,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    34,    48,    17,    18,    31,    52,
      53,    51,    23,    24,    25,    19,    27,    33,    29,    30,
      31,    32,     7,     9,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    83,    48,    45,     0,
      -1,    52,    53,     4,    -1,     6,     7,     4,     9,     6,
       7,     4,     9,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    33
  };

  const signed char
  parser::yystos_[] =
  {
       0,     1,     4,     8,    10,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    48,
      52,    53,    55,    56,    57,    58,    59,    56,    56,    33,
      19,    12,    10,    12,    31,    12,    11,    45,    50,    12,
      17,    47,    49,    41,     0,     4,     6,     7,    17,    18,
      23,    24,    25,    27,    59,     9,    34,    51,    19,    31,
      56,    56,     5,     8,    60,    60,     9
  };

  const signed char
  parser::yyr1_[] =
  {
       0,    54,    55,    55,    55,    55,    56,    56,    56,    56,
      56,    57,    57,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    58,    58,    58,    58,    58,    58,    58,    58,
      58,    58,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    59,    59,    59,    59,    59,    59,    59,
      59,    59,    59,    60,    60
  };

  const signed char
  parser::yyr2_[] =
  {
       0,     2,     0,     1,     2,     1,     3,     3,     3,     3,
       1,     2,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     3,     1,     2,
       2,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       1,     2,     1,     1,     2,     2,     2,     2,     2,     3,
       1,     3,     2,     3,     1
  };


#if SIQUANTDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "BAD_INTEGER",
  "MULTIPLY", "INTEGER", "'/'", "'^'", "'('", "')'", "'y'", "'z'", "'a'",
  "'f'", "'p'", "'n'", "'u'", "'m'", "'c'", "'d'", "'Y'", "'Z'", "'E'",
  "'P'", "'T'", "'G'", "'M'", "'k'", "'h'", "'s'", "'g'", "'r'", "'K'",
  "'o'", "'l'", "'A'", "'H'", "'N'", "'J'", "'W'", "'C'", "'V'", "'F'",
  "'R'", "'S'", "'b'", "'I'", "'x'", "'B'", "'q'", "'v'", "'t'", "'L'",
  "'e'", "$accept", "output", "unit_group", "unit", "prefix",
  "unit_literal", "exponent_group", YY_NULLPTR
  };
#endif


#if SIQUANTDEBUG
  const signed char
  parser::yyrline_[] =
  {
       0,    44,    44,    45,    46,    47,    51,    52,    53,    54,
      55,    59,    60,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   120,   121
  };

  void
  parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // SIQUANTDEBUG

  parser::symbol_kind_type
  parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       8,     9,     2,     2,     2,     2,     2,     6,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    35,    48,    40,     2,    22,
      42,    25,    36,    46,    38,    32,    52,    26,    37,     2,
      23,     2,    43,    44,    24,     2,    41,    39,     2,    20,
      21,     2,     2,     2,     7,     2,     2,    12,    45,    18,
      19,    53,    13,    30,    28,     2,     2,    27,    34,    17,
      15,    33,    14,    49,    31,    29,    51,    16,    50,     2,
      47,    10,    11,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5
    };
    // Last valid token kind.
    const int code_max = 260;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // siquant
#line 1503 "../quantity.tab.cpp"

#line 124 "quantity.y"


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
