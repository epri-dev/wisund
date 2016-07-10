%top{
#define _POSIX_C_SOURCE 1
#include <stdio.h>
}

%{
#include "testmode.h"
%}
%option noyywrap nounput noinput

XDIGIT  [0-9a-fA-F]
ID      [a-z][a-z0-9]*
%%

{XDIGIT}+   { yylval.val = strtol(yytext, (char **)NULL, 16); return HEXBYTE; }
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

[ \t\n]+    { } /* ignore whitespace */
.           { return yytext[0]; }
%%
