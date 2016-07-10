%{
#include <stdio.h>
#include <stdint.h>

extern int yylex(void);

void yyerror (char const *s)
{
    fprintf(stderr, "%s\n", s);
}

void emit(uint8_t byte)
{
    fprintf(stderr, "%2.2x ", byte);
    putchar(byte);
}

void send(const uint8_t *ptr, unsigned len) {
    // wrap the payload inside 0xC0 ... 0xC0 
    emit('\xC0');
    for (  ; len; ++ptr, --len) {
        switch(*ptr) {
            case 0xC0:
                emit(0xDB);
                emit(0xDC);
                break;
            case 0xDB:
                emit(0xDB);
                emit(0xDD);
                break;
            default:
                emit(*ptr);
        }
    }
    emit('\xC0');
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
    |   script command
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
    |       RESTART         { send("\xff", 1); }
    |       HELP            { help(); }
    ;

%%
int main()
{
    return yyparse();
}

