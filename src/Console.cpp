/** 
 *  \file Console.cpp
 *  \brief Implementation of the Console class
 */
#include "Console.h"
#include "scanner.h"
#include "Reply.h"
#include <thread>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <ostream>
#include <vector>

Console::Console(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    trace_scanning{false},
    trace_parsing{false},
    real_quit{false},
    want_reset{false},
    want_echo{false}
{}

Console::~Console() = default;

int Console::runTx(std::istream *in) {
    Scanner scanner(in);
    yy::Parser parser(scanner, *this);
    parser.set_debug_level(trace_parsing);
    int status = parser.parse();
    releaseHold();
    return status;
}

int Console::runRx(std::ostream *out) {
    Message m{};
    while (wantHold() || more()) {
        wait_and_pop(m);
        auto d = decode(m);
        (*out) << d;
        out->flush();
        if (want_echo) {
            std::cout << d;
        }
    }
    return 0;
}

int Console::run(std::istream *in, std::ostream *out) {
    want_reset = false;
    std::thread t1{&Console::runRx, this, out};
    int status = runTx(in);
    t1.join();
    return status;
}

void Console::compound(uint8_t cmd, std::vector<uint8_t> &data)
{
    Message m{data};
    m.insert(m.begin(), cmd);
    m.insert(m.begin(), 0x6);
    push(m);
}

void Console::compound(uint8_t cmd, uint8_t data)
{
    push(Message{0x6, cmd, data});
}

void Console::simple(uint8_t cmd)
{
    push(Message{0x6, cmd});
}

void Console::reset() 
{
    want_reset = true; 
    real_quit = false; 
}

void Console::quit() 
{
    real_quit = true; 
}

bool Console::setEcho(bool echo) 
{
    std::swap(echo, want_echo);
    return echo;
}

bool Console::getQuitValue() const 
{
    return real_quit;
}

bool Console::wantReset() const 
{
    return want_reset;
}
