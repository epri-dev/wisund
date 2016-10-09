%{
#include <cstdint>
#include <iostream>
#include <iomanip>
#include "diag.h"
#include "Message.h"
#include "Serial.h"

static Serial *serial;
extern int yylex(void);

void yyerror (char const *s)
{
    std::cerr << s << '\n';
}

static const Message RestartCmd = Message{0xff}.encode();

void compound(uint8_t cmd, uint8_t data) 
{
    serial->send(Message{0x6, cmd, data}.encode());
}

void simple(uint8_t cmd)
{
    serial->send(Message{0x6, cmd}.encode());
}

void errortxt(void)
{
    std::cerr << "syntax error\n";
}

void help(void)
{
    std::cerr << "Usage: testmode /dev/ttyUSB0\n\n"
        "Accepted commands:\n"
        "fchan nn\nphy nn\ntr51cf\nlbr\nnlbr\nstate\ndiag nn\ngetzz\nping\nrestart\n"
        "help\n\n";
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

command:    FCHAN HEXBYTE   { compound(0x01, $2); }
    |       PHY HEXBYTE     { compound(0x04, $2); }
    |       TR51CF          { simple(0x02); }
    |       LBR             { simple(0x10); }
    |       NLBR            { simple(0x11); }
    |       STATE           { simple(0x20); }
    |       DIAG HEXBYTE    { std::cout << diag(0x21, $2, *serial); }
    |       GETZZ           { simple(0x2F); }
    |       PING            { simple(0x30); }
    |       RESTART         { serial->send(RestartCmd); }
    |       HELP            { help(); }
    |       errors          { errortxt(); }
    ;

errors:     error
    |   errors error
    ;

%%
int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " serialport\n";
        return 1;
    }
    yydebug = 0;
    std::cout << "Opening port " << argv[1] << "\n";
    Serial s{argv[1], 115200};
    serial = &s;
    return yyparse();
}

