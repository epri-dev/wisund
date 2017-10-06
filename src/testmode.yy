%skeleton "lalr1.cc"
%require "3.0"
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
/** 
 *  \file testmode.yy
 *  \brief formal grammar for command-line Console class
 */
#include <cstdint>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <thread>
#include "Message.h"
#include "SafeQueue.h"
#include "Console.h"
#include "scanner.h"

#undef yylex
#define yylex scanner.yylex

void yy::Parser::error(const std::string &s)
{
    std::cout << s << '\n';
}

static const Message RestartCmd = Message{0xff};
static const Message ReportLastCmd = Message{0x07};

static const std::string helpText{
    "Usage: testmode /dev/ttyUSB0\n\n"
    "Accepted commands:\n"
    "fchan nn rr\ntr51cf\nexclude nn ...\nphy nn\npanid nn\n"
    "pansize nn\nroutecost nn\nuseparbs nn\n" 
    "macsec nn\nmaccap nn\n" 
    "lbr\nnlbr\nindex nn\nsetmac macaddr\nbuildid\n"
    "commands accepted in LBR or NLBR active state:\n"
    "state\ndiag nn\nneighbors\nmac\nget nn\nping nn\nlast\nrestart\n"
    "data nn ...\nhelp\nquit\n\n"
};
static const std::vector<uint8_t> helpString{helpText.begin(), helpText.end()};

%}
%define api.value.type variant
%define parse.assert
%expect 1

%token FCHAN TR51CF EXCLUDE PHY PANID LBR NLBR INDEX SETMAC 
%token STATE DIAG BUILDID NEIGHBORS MAC GETZZ PING LAST RESTART 
%token DATA HELP QUIT PAUSE
%token PANSIZE ROUTECOST USEPARBS RANK NETNAME
%token MACSEC MACCAP
%token <uint8_t> HEXBYTE
%type <std::vector<uint8_t>> bytes
%token NEWLINE 
%token <uint8_t> CHAR

%%
script: script command 
    |   command 
    ;

bytes:  bytes HEXBYTE       { $$ = $1; $$.push_back($2); }
    |   HEXBYTE             { $$.push_back($1); }
    ;
    
command:    FCHAN bytes     { console.compound(0x01, $2); }
    |       TR51CF          { console.simple(0x02); }
    |       EXCLUDE bytes   { if (std::none_of($2.begin(), $2.end(), [](uint8_t i){ return i==0; })) {
                                $2.push_back(0);
                                console.compound(0x03, $2); 
                                } else {
                                    std::cout << "Error: exclude channels must not have the value of zero (channel numbering starts at 1)\n";
                                }
                            }
                                
    |       PHY HEXBYTE     { console.compound(0x04, $2); }
    |       PANID bytes     {if ($2.size() == 2) {
                                console.compound(0x06, $2); 
                                } else {
                                    std::cout << "Error: panid  must have 2 bytes\n";
                                }  // 2 octets for PANID
                            }
    |       PANSIZE bytes   {if ($2.size() == 2) {
                                console.compound(0x41, $2); 
                                } else {
                                    std::cout << "Error: pansize  must have 2 bytes\n";
                                }  // 2 octets for PANID
                            }
    |       ROUTECOST bytes {if ($2.size() == 2) {
                                console.compound(0x45, $2); 
                                } else {
                                    std::cout << "Error: routecost  must have 2 bytes\n";
                                }  // 2 octets for PANID
                            }
    |       USEPARBS HEXBYTE { console.compound(0x42, $2); }
    |       RANK HEXBYTE    { console.compound(0x46, $2); }
    |       MACSEC HEXBYTE  { console.compound(0x47, $2); }
    |       MACCAP HEXBYTE  { console.compound(0x31, $2); }
    |       NETNAME bytes   { if ($2.size() >= 2) {
                                console.compound(0x44, $2); 
                                } else {
                                    std::cout << "Error: netname must be at least 2 bytes\n";
                                } 
                            }  
    |       LBR             { console.simple(0x10); }
    |       NLBR            { console.simple(0x11); }
    |       INDEX HEXBYTE   { console.compound(0x12, $2); } 
    |       DATA bytes      { console.push(Message{$2}); }
    |       SETMAC bytes    { if ($2.size() == 8) {
                                console.compound(0x13, $2); 
                                } else {
                                    std::cout << "Error: mac must have 8 bytes\n";
                                } 
                            }  
    |       STATE           { console.simple(0x20); }
    |       DIAG HEXBYTE    { console.compound(0x21, $2); }
    |       BUILDID         { console.simple(0x22); }
    |       NEIGHBORS       { console.simple(0x23); }
    |       MAC             { console.simple(0x24); }
    |       GETZZ HEXBYTE   { console.compound(0x2F, $2); }
    |       PING HEXBYTE    { console.compound(0x30, $2); }
    |       LAST            { console.push(ReportLastCmd); }
    |       RESTART         { console.push(RestartCmd); }
    |       HELP            { console.selfInput(helpString); }
    |       PAUSE HEXBYTE   { std::this_thread::sleep_for(std::chrono::milliseconds(100 * $2)); }
    |       QUIT            { console.quit(); return 0; }
    |       NEWLINE         { }
    |       CHAR            { console.reset(); return 0; }
    |       errors          { error("unknown command"); }
    ;

errors:     errors error
    |   error
    ;

%%

