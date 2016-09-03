%{
#define _POSIX_C_SOURCE 2
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "slip.h"
#include "serialtun.h"

extern int yylex(void);
extern int opterr, optopt;
const char *serialdev = "/dev/ttyAMA0";

static uint8_t ResetCmd[] = { 0xFF };
static bool do_slip = false;

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
    slipsend(STDOUT_FILENO, buffer, 3);
}

void simple(uint8_t cmd)
{
    uint8_t buffer[2];
    buffer[0] = 0x6;
    buffer[1] = cmd;
    slipsend(STDOUT_FILENO, buffer, 2);
}


void help(void)
{
    fprintf(stderr, "Usage: testmode > /dev/ttyUSB0\n\n"
        "Accepted commands:\n"
        "fchan nn\nphy nn\ntr51cf\nlbr\nnlbr\nstate\ndiag\ngetzz\nping\nrestart\n"
        "slip\nhelp\n\n"
    );
}

%}

%union {
    int val;
}
%token FCHAN TR51CF PHY LBR NLBR STATE DIAG GETZZ PING RESTART HELP SLIP
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
    |       RESTART         { slipsend(STDOUT_FILENO, ResetCmd, sizeof(ResetCmd)); }
    |       HELP            { help(); }
    |       SLIP            { transcomm(do_slip, serialdev); }
    |       errors          { errortxt(); }
    ;

%%
int main(int argc, char *argv[])
{
    int c;
    opterr = 0;

    while ((c = getopt(argc, argv, "s")) != -1) {
        switch(c) {
            case 's':
                do_slip = true;
                break;
            case '?':
                fprintf(stderr, "Unknown option character \\x%x\n", optopt);
                break;
            default:
                fprintf(stderr, "Unknown error\n");
                exit(1);
        }
    }
        
    return yyparse();
}

