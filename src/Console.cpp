#include "Console.h"
#include "scanner.h"
#include <thread>


Console::Console(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    trace_scanning{false},
    trace_parsing{false}
{}

Console::~Console() = default;

int Console::runTx(std::istream *in) {
    Scanner scanner(in);
    yy::Parser parser(scanner, *this);
    parser.set_debug_level(trace_parsing);
    int status = parser.parse();
    std::cout << "Console parser ended with status " << status << "\n";
    releaseHold();
    return status;
}

int Console::runRx(std::ostream *out) {
    Message m{0};
    while (wantHold() || more()) {
        *out << "Console waiting for message\n";
        wait_and_pop(m);
        *out << "Console received message: " << m << '\n';
    }
    return 0;
}

int Console::run(std::istream *in, std::ostream *out) {
    std::thread t1{&Console::runRx, this, out};
    int status = runTx(in);
    t1.join();
    return status;
}

void Console::compound(uint8_t cmd, uint8_t data)
{
    push(Message{0x6, cmd, data});
}

void Console::simple(uint8_t cmd)
{
    push(Message{0x6, cmd});
}
