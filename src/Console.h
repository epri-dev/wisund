#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __linux__
#include <unistd.h>
#endif
#include "Device.h"
#include "SafeQueue.h"
#include "parser.h"

class Console : public Device {
public:
    Console(SafeQueue<Message> &input, SafeQueue<Message> &output);
    virtual ~Console();

    // lexer control functions
    void scan_begin();
    void scan_end();
    bool trace_scanning;

    void compound(uint8_t cmd, uint8_t data);
    void simple(uint8_t cmd);

    int run();
    bool trace_parsing;

    // error handling
//    void error(const yy::location &loc, std::string &msg);
    void error(std::string &msg);
};

#endif // CONSOLE_H
