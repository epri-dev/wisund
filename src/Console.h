#ifndef CONSOLE_H
#define CONSOLE_H

#ifdef __linux__
#include <unistd.h>
#endif
#include "Message.h"
#include "Device.h"
#include "SafeQueue.h"
#include <vector>

/**
 * wrapper class for the parser/lexer code.
 *
 * This class gets text input from the input stream (passed to `run`)
 * and converts into the messages that are sent to the radio.  It also 
 * converts the messages received from the radio into human readable form.
 * (Specifically, replies are rendered as JSON.)
 */
class Console : public Device {
public:
    /// constructor takes references to input and output queues
    Console(SafeQueue<Message> &input, SafeQueue<Message> &output);
    /// destructor is virtual in case class needs to be further derived
    virtual ~Console();
    /// prints passed error message to `std::cerr`
    static void error(std::string &msg);
    /// emits a compound command Message to the output queue
    void compound(uint8_t cmd, std::vector<uint8_t> &data);
    /// emits a compound command Message to the output queue
    void compound(uint8_t cmd, uint8_t data);
    /// emits a simple command Message to the output queue
    void simple(uint8_t cmd);
    /// runs the transmit handler (converting text commands to command Messages)
    int runTx(std::istream *in = &std::cin);
    /// runs the receive handler (converting received Messages to JSON text output)
    int runRx(std::ostream *out = &std::cout);
    /// runs both the receive and transmit handlers in required sequence
    int run(std::istream *in, std::ostream *out);

private:
    void decode(const Message &msg, std::ostream &out);
    bool trace_scanning;
    bool trace_parsing;
};

#endif // CONSOLE_H
