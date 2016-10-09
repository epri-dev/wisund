%top{
#define _POSIX_C_SOURCE 1
#include <cstdlib>
#ifdef __linux__
#include <unistd.h>
#endif
}

%{
#include "testmode.hpp"
%}
%option noyywrap nounput noinput nounistd

XDIGIT  [0-9a-fA-F]
ID      [a-z][a-z0-9]*
%%

{XDIGIT}+   { yylval.val = std::strtol(yytext, nullptr, 16); return HEXBYTE; }
fchan       { return FCHAN; }
tr51cf      { return TR51CF; }
phy         { return PHY; }
lbr         { return LBR; }
nlbr        { return NLBR; }
state       { return STATE; }
diag        { return DIAG; }
getzz       { return GETZZ; }
ping        { return PING; }
restart     { return RESTART; }
help        { return HELP; }

[ \t]+      { } /* ignore whitespace */
\n          { return '\n'; }
.           { return yytext[0]; }
%%
