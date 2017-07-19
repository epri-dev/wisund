#ifndef SCANNER_H
#define SCANNER_H
/** 
 *  \file scanner.h
 *  \brief Interface for the Scanner class
 */

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "testmode.hpp"

/**
 * A wrapper class for the lexer that tokenizes text input
 */
class Scanner : public yyFlexLexer {
public:    
    /// The default constructor.
    Scanner() : yyFlexLexer() {}
    /// Create a scanner that parses the passed `istream`
    Scanner(std::istream *in) : yyFlexLexer(in) {}
    using FlexLexer::yylex;
    /// the yylex function is automatically created by Flex.
    virtual int yylex(yy::Parser::semantic_type *lval);
private:
    /// pointer to the current value
    yy::Parser::semantic_type *yylval = nullptr; 
};

#endif // SCANNER_H
