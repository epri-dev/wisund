#ifndef SCANNER_H
#define SCANNER_H

#if !defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "testmode.hpp"

class Scanner : public yyFlexLexer {
public:
    Scanner() : yyFlexLexer() {}
    using FlexLexer::yylex;
    virtual int yylex(yy::Parser::semantic_type *lval);
private:
    yy::Parser::semantic_type *yylval = nullptr; 
};

#endif // SCANNER_H
