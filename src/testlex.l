%top{
}

%{
#include <cstdlib>
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

{XDIGIT}{XDIGIT}   { uint8_t val = (0xffu & std::stoi(yytext, 0, 16));
                yylval->build(val); 
                return token::HEXBYTE;
            }
fchan       { return token::FCHAN; }
tr51cf      { return token::TR51CF; }
exclude     { return token::EXCLUDE; }
phy         { return token::PHY; }
panid       { return token::PANID; }
lbr         { return token::LBR; }
nlbr        { return token::NLBR; }
index       { return token::INDEX; }
setmac      { return token::SETMAC; }
state       { return token::STATE; }
diag        { return token::DIAG; }
buildid     { return token::BUILDID; }
neighbors   { return token::NEIGHBORS; }
mac         { return token::MAC; }
getzz       { return token::GETZZ; }
ping        { return token::PING; }
last        { return token::LAST; }
restart     { return token::RESTART; }
data        { return token::DATA; }
help        { return token::HELP; }
quit|exit   { return token::QUIT; }

[ \t\n]+      { } /* ignore whitespace */
\n          { } /* return token::NEWLINE; } */
.           { return token::CHAR; }
%%
