%top{
}

%{
#include "Console.h"
#include "testmode.hpp"
#include "scanner.h"
#undef YY_DECL
#define YY_DECL int Scanner::yylex(yy::Parser::semantic_type * lval)
using token = yy::Parser::token;
%}
%option noyywrap nounput noinput nounistd c++
%option yyclass="Scanner"

XDIGIT  [0-9a-fA-F]
ID      [a-z][a-z0-9]*
%%
%{
        yylval = lval;
%}

{XDIGIT}+   { long val = std::strtol(yytext, nullptr, 16); 
                return yylval->build(val); }
fchan       { return token::FCHAN; }
tr51cf      { return token::TR51CF; }
phy         { return token::PHY; }
lbr         { return token::LBR; }
nlbr        { return token::NLBR; }
state       { return token::STATE; }
diag        { return token::DIAG; }
buildid     { return token::BUILDID; }
getzz       { return token::GETZZ; }
ping        { return token::PING; }
restart     { return token::RESTART; }
help        { return token::HELP; }

[ \t]+      { } /* ignore whitespace */
\n          { return token::NEWLINE; }
.           { return token::CHAR; }
%%
