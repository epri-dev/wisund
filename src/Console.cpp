#include "Console.h"
#include "scanner.h"


Console::Console(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    trace_scanning{false},
    trace_parsing{false}
{}

Console::~Console() = default;

int Console::run() {
    Scanner scanner;
    yy::Parser parser(scanner, *this);

    parser.set_debug_level(trace_parsing);
    return parser.parse();
}

void Console::compound(uint8_t cmd, uint8_t data)
{
    push(Message{0x6, cmd, data});
}

void Console::simple(uint8_t cmd)
{
    push(Message{0x6, cmd});
}
