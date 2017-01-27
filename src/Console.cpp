#include "Console.h"
#include "scanner.h"
#include "Reply.h"
#include <thread>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <vector>


Console::Console(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    trace_scanning{false},
    trace_parsing{false},
    real_quit{false}
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
        decode(m, *out);
        // std::cout << decode(m);
        out->flush();
    }
    return 0;
}

int Console::run(std::istream *in, std::ostream *out) {
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

void Console::quit() 
{
    real_quit = true; 
}

bool Console::getQuitValue() const 
{
    return real_quit;
}
