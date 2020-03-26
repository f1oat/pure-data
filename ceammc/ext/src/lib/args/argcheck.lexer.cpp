// argcheck.lexer.cpp generated by reflex 1.6.3 from argcheck.l

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_YYLTYPE             ceammc::location
#define REFLEX_OPTION_YYSTYPE             ceammc::ArgCheckParser::semantic_type
#define REFLEX_OPTION_bison_cc            true
#define REFLEX_OPTION_bison_cc_namespace  ceammc
#define REFLEX_OPTION_bison_cc_parser     ArgCheckParser
#define REFLEX_OPTION_bison_complete      true
#define REFLEX_OPTION_freespace           true
#define REFLEX_OPTION_header_file         "argcheck.lexer.h"
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               ArgCheckLexer
#define REFLEX_OPTION_namespace           ceammc
#define REFLEX_OPTION_noyywrap            true
#define REFLEX_OPTION_outfile             "argcheck.lexer.cpp"
#define REFLEX_OPTION_reentrant           true
#define REFLEX_OPTION_token_eof           ceammc::ArgCheckParser::symbol_type(0)
#define REFLEX_OPTION_token_type          ceammc::ArgCheckParser::symbol_type
#define REFLEX_OPTION_unicode             true

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top{ user code %}                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "argcheck.l"

    # include <string>

    # include "argcheck.parser.hpp"
    using token = ceammc::ArgCheckParser::token;


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  REGEX MATCHER                                                             //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/matcher.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  ABSTRACT LEXER CLASS                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <reflex/abslexer.h>

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  LEXER CLASS                                                               //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

namespace ceammc {

class ArgCheckLexer : public reflex::AbstractLexer<reflex::Matcher> {
#line 8 "argcheck.l"

    public:
        long num_int;

 public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  ArgCheckLexer(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
  }
  static const int INITIAL = 0;
  virtual ceammc::ArgCheckParser::symbol_type lex(void);
};

} // namespace ceammc

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %{ user code %}                                                //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 18 "argcheck.l"
/*%option graphs-file*/
/*%option debug*/

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  BISON C++                                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 2: rules                                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

ceammc::ArgCheckParser::symbol_type ceammc::ArgCheckLexer::lex()
{
  static const char *REGEX_INITIAL = "(?mx)(a)|(b)|(d)|(f)|(s)|(i)|(i(?=(?:(?:[\\x2b\\x2d]?(?:0|[1-9][0-9]*))\\.\\.(?:[\\x2b\\x2d]?(?:0|[1-9][0-9]*)))))|(\\+)|(\\*)|(\\?)|(\\[)|(\\])|(\\()|(\\))|(\\|)|(%)|(<=)|(<)|(>=)|(>)|(!=)|(=)|(~)|(\\^)|(\\^2)|(\\$)|((?:[\\x09\\x0a\\x20]+))|((?:\\.\\.(?=(?:(?:[\\x2b\\x2d]?(?:0|[1-9][0-9]*))|]))))|((?:[\\x2b\\x2d]?(?:0|[1-9][0-9]*)))|((?:(?:[\\x2b\\x2d]?(?:0|[1-9][0-9]*))(?:\\.[0-9]+)?))|((?:[\\x2eA-Z_a-z][\\x2e0-9A-Z_a-z]*))|((?:(?:')(?:(?:[\\x00-d]|[f-n]|p|r|[v-z]|\\x7c|[~\\x7f]|[\\xc2-\\xdf][\\x80-\\xbf]|\\xe0[\\xa0-\\xbf][\\x80-\\xbf]|[\\xe1-\\xec][\\x80-\\xbf][\\x80-\\xbf]|\\xed[\\x80-\\x9f][\\x80-\\xbf]|[\\xee\\xef][\\x80-\\xbf][\\x80-\\xbf]|\\xf0[\\x90-\\xbf][\\x80-\\xbf][\\x80-\\xbf]|[\\xf1-\\xf3][\\x80-\\xbf][\\x80-\\xbf][\\x80-\\xbf]|\\xf4[\\x80-\\x8f][\\x80-\\xbf][\\x80-\\xbf])|`(?:'))*(?:')))|((?:(?:\")(?:(?:[\\x00-c]|[f-n]|p|[rs]|[v-z]|\\x7c|[~\\x7f]|[\\xc2-\\xdf][\\x80-\\xbf]|\\xe0[\\xa0-\\xbf][\\x80-\\xbf]|[\\xe1-\\xec][\\x80-\\xbf][\\x80-\\xbf]|\\xed[\\x80-\\x9f][\\x80-\\xbf]|[\\xee\\xef][\\x80-\\xbf][\\x80-\\xbf]|\\xf0[\\x90-\\xbf][\\x80-\\xbf][\\x80-\\xbf]|[\\xf1-\\xf3][\\x80-\\xbf][\\x80-\\xbf][\\x80-\\xbf]|\\xf4[\\x80-\\x8f][\\x80-\\xbf][\\x80-\\xbf])|`(?:\"))*(?:\")))|(.)";
  static const reflex::Pattern PATTERN_INITIAL(REGEX_INITIAL);
  if (!has_matcher())
  {
    matcher(new Matcher(PATTERN_INITIAL, stdinit(), this));
  }
  while (true)
  {
        switch (matcher().scan())
        {
          case 0:
            if (matcher().at_end())
            {
              return ceammc::ArgCheckParser::symbol_type(0);
            }
            else
            {
              out().put(matcher().input());
            }
            break;
          case 1: // rule at line 44: a
#line 44 "argcheck.l"
{ return token::TATOM; }
            break;
          case 2: // rule at line 45: b
#line 45 "argcheck.l"
{ return token::TBOOL; }
            break;
          case 3: // rule at line 46: d
#line 46 "argcheck.l"
{ return token::TDATA; }
            break;
          case 4: // rule at line 47: f
#line 47 "argcheck.l"
{ return token::TFLOAT; }
            break;
          case 5: // rule at line 48: s
#line 48 "argcheck.l"
{ return token::TSYMBOL; }
            break;
          case 6: // rule at line 49: i
#line 49 "argcheck.l"
{ return token::TINT; } // for simple i types
            break;
          case 7: // rule at line 50: i(?=(?:(?:[\x2b\x2d]?(?:0|[1-9][0-9]*))\.\.(?:[\x2b\x2d]?(?:0|[1-9][0-9]*))))
#line 50 "argcheck.l"
{ return token::TINT; } // to prevent {symbol} longest match in i2..4 case
            break;
          case 8: // rule at line 51: \+
#line 51 "argcheck.l"
{ return token::PLUS; }
            break;
          case 9: // rule at line 52: \*
#line 52 "argcheck.l"
{ return token::ASTERISK; }
            break;
          case 10: // rule at line 53: \?
#line 53 "argcheck.l"
{ return token::QUESTION; }
            break;
          case 11: // rule at line 54: \[
#line 54 "argcheck.l"
{ return token::REPEAT_START; }
            break;
          case 12: // rule at line 55: \]
#line 55 "argcheck.l"
{ return token::REPEAT_END; }
            break;
          case 13: // rule at line 56: \(
#line 56 "argcheck.l"
{ return token::GROUP_START; }
            break;
          case 14: // rule at line 57: \)
#line 57 "argcheck.l"
{ return token::GROUP_END; }
            break;
          case 15: // rule at line 58: \|
#line 58 "argcheck.l"
{ return token::OR; }
            break;
          case 16: // rule at line 59: %
#line 59 "argcheck.l"
{ return token::MODULUS; }
            break;
          case 17: // rule at line 60: <=
#line 60 "argcheck.l"
{ return token::LE; }
            break;
          case 18: // rule at line 61: <
#line 61 "argcheck.l"
{ return token::LT; }
            break;
          case 19: // rule at line 62: >=
#line 62 "argcheck.l"
{ return token::GE; }
            break;
          case 20: // rule at line 63: >
#line 63 "argcheck.l"
{ return token::GT; }
            break;
          case 21: // rule at line 64: !=
#line 64 "argcheck.l"
{ return token::NE; }
            break;
          case 22: // rule at line 65: =
#line 65 "argcheck.l"
{ return token::EQ; }
            break;
          case 23: // rule at line 66: ~
#line 66 "argcheck.l"
{ return token::TILDE; }
            break;
          case 24: // rule at line 67: \^
#line 67 "argcheck.l"
{ return token::CAPS; }
            break;
          case 25: // rule at line 68: \^2
#line 68 "argcheck.l"
{ return token::POWER_OF_TWO; }
            break;
          case 26: // rule at line 69: \$
#line 69 "argcheck.l"
{ return token::DOLLAR; }
            break;
          case 27: // rule at line 70: (?:[\x09\x0a\x20]+)
#line 70 "argcheck.l"
{ ; }
            break;
          case 28: // rule at line 71: (?:\.\.(?=(?:(?:[\x2b\x2d]?(?:0|[1-9][0-9]*))|])))
#line 71 "argcheck.l"
{ return token::REPEAT_RANGE; }
            break;
          case 29: // rule at line 72: (?:[\x2b\x2d]?(?:0|[1-9][0-9]*))
#line 72 "argcheck.l"
{ return ArgCheckParser::make_INTEGER(std::strtol(text(), 0, 10)); }
            break;
          case 30: // rule at line 73: (?:(?:[\x2b\x2d]?(?:0|[1-9][0-9]*))(?:\.[0-9]+)?)
#line 73 "argcheck.l"
{ return ArgCheckParser::make_FLOAT(std::strtod(text(), 0)); }
            break;
          case 31: // rule at line 74: (?:[\x2eA-Z_a-z][\x2e0-9A-Z_a-z]*)
#line 74 "argcheck.l"
{ return ArgCheckParser::make_SYMBOL(text()); }

            break;
          case 32: // rule at line 76: (?:(?:')(?:(?:[\x00-d]|[f-n]|p|r|[v-z]|\x7c|[~\x7f]|[\xc2-\xdf][\x80-\xbf]|\xe0[\xa0-\xbf][\x80-\xbf]|[\xe1-\xec][\x80-\xbf][\x80-\xbf]|\xed[\x80-\x9f][\x80-\xbf]|[\xee\xef][\x80-\xbf][\x80-\xbf]|\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]|\xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf])|`(?:'))*(?:'))
#line 76 "argcheck.l"
{
                    const auto N = strlen(text()) - 2;
                    return ArgCheckParser::make_STRING(std::string(text() + 1, N));
                }

            break;
          case 33: // rule at line 81: (?:(?:")(?:(?:[\x00-c]|[f-n]|p|[rs]|[v-z]|\x7c|[~\x7f]|[\xc2-\xdf][\x80-\xbf]|\xe0[\xa0-\xbf][\x80-\xbf]|[\xe1-\xec][\x80-\xbf][\x80-\xbf]|\xed[\x80-\x9f][\x80-\xbf]|[\xee\xef][\x80-\xbf][\x80-\xbf]|\xf0[\x90-\xbf][\x80-\xbf][\x80-\xbf]|[\xf1-\xf3][\x80-\xbf][\x80-\xbf][\x80-\xbf]|\xf4[\x80-\x8f][\x80-\xbf][\x80-\xbf])|`(?:"))*(?:"))
#line 81 "argcheck.l"
{
                    const auto N = strlen(text()) - 2;
                    return ArgCheckParser::make_STRING(std::string(text() + 1, N));
                }

            break;
          case 34: // rule at line 86: .
#line 86 "argcheck.l"
{ return token::LEXER_ERR; /* error */ }

            break;
        }
  }
}

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 3: user code                                                      //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 89 "argcheck.l"

