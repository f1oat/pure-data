// data_string.lexer.h generated by reflex 2.1.5 from data_string.l

#ifndef REFLEX_DATA_STRING_LEXER_H
#define REFLEX_DATA_STRING_LEXER_H
#define IN_HEADER 1
#define REFLEX_VERSION "2.1.5"

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  OPTIONS USED                                                              //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#define REFLEX_OPTION_YYLTYPE             ceammc::ds::location
#define REFLEX_OPTION_YYSTYPE             ceammc::ds::DataStringParser::semantic_type
#define REFLEX_OPTION_bison_cc            true
#define REFLEX_OPTION_bison_cc_namespace  ceammc::ds
#define REFLEX_OPTION_bison_cc_parser     DataStringParser
#define REFLEX_OPTION_bison_complete      true
#define REFLEX_OPTION_bison_locations     true
#define REFLEX_OPTION_fast                true
#define REFLEX_OPTION_freespace           true
#define REFLEX_OPTION_header_file         "data_string.lexer.h"
#define REFLEX_OPTION_lex                 lex
#define REFLEX_OPTION_lexer               DataStringLexer
#define REFLEX_OPTION_namespace           ceammc::ds
#define REFLEX_OPTION_noyywrap            true
#define REFLEX_OPTION_outfile             "data_string.lexer.cpp"
#define REFLEX_OPTION_reentrant           true
#define REFLEX_OPTION_token_eof           ceammc::ds::DataStringParser::symbol_type(0, location())
#define REFLEX_OPTION_token_type          ceammc::ds::DataStringParser::symbol_type
#define REFLEX_OPTION_unicode             true

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  SECTION 1: %top user code                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#line 1 "data_string.l"

    # include <string>
    # include <memory>

    # include "lex/data_string.parser.hpp"
    # include "lex/data_string.location.hpp"
    # include "ceammc_log.h"

    using token = ceammc::ds::DataStringParser::token;


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
namespace ds {

class DataStringLexer : public reflex::AbstractLexer<reflex::Matcher> {
#line 12 "data_string.l"

    public:
        size_t output_indent = {0};

        std::string indent(size_t n = 0) const {
            return std::string(output_indent + n, ' ');
        }

 public:
  typedef reflex::AbstractLexer<reflex::Matcher> AbstractBaseLexer;
  DataStringLexer(
      const reflex::Input& input = reflex::Input(),
      std::ostream&        os    = std::cout)
    :
      AbstractBaseLexer(input, os)
  {
  }
  static const int INITIAL = 0;
  std::string filename;
  virtual ceammc::ds::location location(void)
  {
    ceammc::ds::location yylloc;
    yylloc.begin.filename = &filename;
    yylloc.begin.line = static_cast<unsigned int>(matcher().lineno());
    yylloc.begin.column = static_cast<unsigned int>(matcher().columno());
    yylloc.end.filename = &filename;
    yylloc.end.line = static_cast<unsigned int>(matcher().lineno_end());
    yylloc.end.column = static_cast<unsigned int>(matcher().columno_end());
    return yylloc;
  }
  virtual ceammc::ds::DataStringParser::symbol_type lex(void);
};

} // namespace ceammc
} // namespace ds

////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//  BISON C++                                                                 //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#endif
