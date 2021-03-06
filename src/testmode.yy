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
// ===========================================================================
// Copyright (c) 2017, Electric Power Research Institute (EPRI)
// All rights reserved.
//
// wisund ("this software") is licensed under BSD 3-Clause license.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// *  Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// *  Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// *  Neither the name of EPRI nor the names of its contributors may
//    be used to endorse or promote products derived from this software without
//    specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
// WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
// OF SUCH DAMAGE.
//
// This EPRI software incorporates work covered by the following copyright and permission
// notices. You may not use these works except in compliance with their respective
// licenses, which are provided below.
//
// These works are provided by the copyright holders and contributors "as is" and any express or
// implied warranties, including, but not limited to, the implied warranties of merchantability
// and fitness for a particular purpose are disclaimed.
//
// This software relies on the following libraries and licenses:
//
// ###########################################################################
// Boost Software License, Version 1.0
// ###########################################################################
//
// * asio v1.10.8 (https://sourceforge.net/projects/asio/files/)
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.
// 

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
    "macsec nn\nmaccap \ncapfile filename\n" 
    "lbr\nnlbr\nindex nn\nsetmac macaddr\nbuildid\n"
    "commands accepted in LBR or NLBR active state:\n"
    "state\ndiag nn\nneighbors\nmac\nget nn\nping nn\nlast\nrestart\n"
    "data nn ...\nhelp\nquit\n\n"
};
static const std::vector<uint8_t> helpString{helpText.begin(), helpText.end()};

%}
%define api.value.type variant
%define parse.assert

%token FCHAN TR51CF EXCLUDE PHY PANID LBR NLBR INDEX SETMAC 
%token STATE DIAG BUILDID NEIGHBORS MAC GETZZ PING LAST RESTART 
%token DATA HELP QUIT PAUSE PERIOD CAPFILE
%token PANSIZE ROUTECOST USEPARBS RANK NETNAME
%token MACSEC MACCAP DIVIDER
%token <std::string> ID
%token <uint8_t> HEXBYTE
%type <std::string> path
%type <std::string> filename 
%type <std::string> pathexpr
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

path:   pathexpr filename   { $$ = $1 + $2; }
    ;

pathexpr:   pathexpr PERIOD PERIOD DIVIDER  { $$ = $1; $$ += "../"; }
    |       pathexpr PERIOD DIVIDER         { $$ = $1; $$ += "./"; }
    |       pathexpr ID DIVIDER             { $$ = $1; $$ += $2 + "/"; }
    |       DIVIDER                         { $$ = "/"; }
    |                                       { $$ = ""; }
    ;

filename:   ID PERIOD ID    { $$ = $1 + "." + $3; }
    |       ID              { $$ = $1; }
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
    |       CAPFILE path    { std::vector<uint8_t> v{std::begin($2), std::end($2)};
                                console.control(0x01, v); }
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
    |       error           { error("unknown command"); }
    ;

%%

