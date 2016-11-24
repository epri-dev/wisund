%skeleton "lalr1.cc"
%require "3.0.4"
%debug
%defines
%define parser_class_name {Parser}

%code requires {
    class Scanner;

// The following definitions is missing when %locations isn't used
# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

}
%parse-param {Scanner &scanner} 
%parse-param {Console &console} 

%{
#include <cstdint>
#include <iostream>
#include <iomanip>
#include "Message.h"
#include "SafeQueue.h"
#include "Console.h"
#include "scanner.h"

#undef yylex
#define yylex scanner.yylex

void yy::Parser::error(const std::string &s)
{
    std::cerr << s << '\n';
}

static const Message RestartCmd = Message{0xff};
static const Message ReportLastCmd = Message{0x07};

static void errortxt(void)
{
    std::cerr << "syntax error\n";
}

static void help(void)
{
    std::cerr << "Usage: testmode /dev/ttyUSB0\n\n"
        "Accepted commands:\n"
        "fchan nn\ntr51cf\nexclude nn ...\nphy nn\npanid nn\n"
        "lbr\nnlbr\nindex nn\nsetmac macaddr\nbuildid\n"
        "commands accepted in LBR or NLBR active state:\n"
        "state\ndiag nn\nneighbors\nmac\nget nn\nping nn\nlast\nrestart\n"
        "help\nquit\n\n";
}

%}
%define api.value.type variant
%define parse.assert

%token FCHAN TR51CF EXCLUDE PHY PANID LBR NLBR INDEX SETMAC 
%token STATE DIAG BUILDID NEIGHBORS MAC GETZZ PING LAST RESTART 
%token HELP QUIT 
%token <int> HEXBYTE
%token NEWLINE CHAR

%%
script:     /* empty */
    |   NEWLINE
    |   script command NEWLINE

bytes:  HEXBYTE             // TODO: construct byte array for EXCLUDE command
    |   bytes HEXBYTE
    
command:    FCHAN HEXBYTE   { console.compound(0x01, $2); }
    |       TR51CF          { console.simple(0x02); }
    |       EXCLUDE bytes   { console.compound(0x03, 0); }  // TODO: handle list
    |       PHY HEXBYTE     { console.compound(0x04, $2); }
    |       PANID HEXBYTE   { console.compound(0x06, $2); }  // TODO: verify this
    |       LBR             { console.simple(0x10); }
    |       NLBR            { console.simple(0x11); }
    |       INDEX HEXBYTE   { console.compound(0x12, $2); } 
    |       SETMAC bytes    { console.compound(0x13, 0); }  // TODO: handle list
    |       STATE           { console.simple(0x20); }
    |       DIAG HEXBYTE    { console.compound(0x21, $2); }
    |       BUILDID         { console.simple(0x22); }
    |       NEIGHBORS       { console.simple(0x23); }
    |       MAC             { console.simple(0x24); }
    |       GETZZ HEXBYTE   { console.compound(0x2F, $2); }
    |       PING HEXBYTE    { console.compound(0x30, $2); }
    |       LAST            { console.push(ReportLastCmd); }
    |       RESTART         { console.push(RestartCmd); }
    |       HELP            { help(); }
    |       QUIT            { return 0; }
    |       errors          { errortxt(); }
    ;

errors:     error
    |   errors error
    ;

%%

