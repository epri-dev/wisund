%{
#include <stdio.h>
#include <stdint.h>
#include "slip.h"

extern int yylex(void);

void yyerror (char const *s)
{
    fprintf(stderr, "%s\n", s);
}

void errortxt(void)
{
    fprintf(stderr, "syntax error\n");
}

void compound(uint8_t cmd, uint8_t data) 
{
    uint8_t buffer[3];
    buffer[0] = 0x6;
    buffer[1] = cmd;
    buffer[2] = data;
    send(buffer, 3);
}

void simple(uint8_t cmd)
{
    uint8_t buffer[2];
    buffer[0] = 0x6;
    buffer[1] = cmd;
    send(buffer, 2);
}

static uint8_t ResetCmd[] = { 0xFF };

void help(void)
{
    fprintf(stderr, "Usage: testmode > /dev/ttyUSB0\n\n"
        "Accepted commands:\n"
        "fchan nn\nphy nn\ntr51cf\nlbr\nnlbr\nstate\ndiag\ngetzz\nping\nrestart\n"
        "help\n\n"
    );
}

%}

%union {
    int val;
}
%token FCHAN TR51CF PHY LBR NLBR STATE DIAG GETZZ PING RESTART HELP
%token <val> HEXBYTE

%%
script:     /* empty */
    |   script command '\n'
    ;

errors:     error
    |       errors error
    ;

command:    FCHAN HEXBYTE   { compound(0x01, $2); }
    |       PHY HEXBYTE     { compound(0x04, $2); }
    |       TR51CF          { simple(0x02); }
    |       LBR             { simple(0x10); }
    |       NLBR            { simple(0x11); }
    |       STATE           { simple(0x20); }
    |       DIAG            { simple(0x21); }
    |       GETZZ           { simple(0x2F); }
    |       PING            { simple(0x30); }
    |       RESTART         { send(ResetCmd, sizeof(ResetCmd)); }
    |       HELP            { help(); }
    |       errors          { errortxt(); }
    ;

%%
int main()
{
    return yyparse();
}

