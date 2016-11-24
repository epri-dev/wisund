#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __linux__
#include <unistd.h>
#endif
#include "Message.h"
#include "Device.h"
#include "SafeQueue.h"
#include "parser.h"
#include <vector>

class Console : public Device {
public:
    Console(SafeQueue<Message> &input, SafeQueue<Message> &output);
    virtual ~Console();
    void error(std::string &msg);
    void compound(uint8_t cmd, std::vector<uint8_t> &data);
    void compound(uint8_t cmd, uint8_t data);
    void simple(uint8_t cmd);
    int runTx(std::istream *in = &std::cin);
    int runRx(std::ostream *out = &std::cout);
    int run(std::istream *in, std::ostream *out);

private:
    void decode(const Message &msg, std::ostream &out);
    bool trace_scanning;
    bool trace_parsing;
};

#endif // CONSOLE_H
