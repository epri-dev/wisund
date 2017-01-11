#include <iostream>
#include <thread>
#include "SafeQueue.h"
#include "Console.h"
#include "SerialDevice.h"
#include "TunDevice.h"

class Router : public Device 
{
public:
    /// unlike other Devices, the Router two output queues; one for raw and another for all other 
    Router(SafeQueue<Message> &input, SafeQueue<Message> &output, SafeQueue<Message> &rawOutput);
    virtual ~Router();
    int run(std::istream *in, std::ostream *out);
private:
    SafeQueue<Message> &rawQ;
};

Router::Router(SafeQueue<Message> &input, SafeQueue<Message> &output, SafeQueue<Message> &rawOutput) :
    Device{input, output},
    rawQ{rawOutput}
{}

Router::~Router() = default;

int Router::run(std::istream *in, std::ostream *out)
{
    in = in;
    out = out;
    Message m{0};
    while (wantHold()) {
        wait_and_pop(m);
        if (m.isRaw()) {
            rawQ.push(m);
        } else {
            push(m);
        }
    }
    return 0;
}

/*
 * The router has a few simple rules.
 *
 * On Windows there is no TUN device so the serial port and
 * console are directly tied together with no routing decisions.
 *
 * On Linux, the rules are:
 *    1. Everything from the Console goes to the serial port
 *    2. Everything from the TUN goes to the serial port
 *    3. If a raw packet comes from the serial port, it goes to the TUN
 *    4. All non-raw packets from the serial port goes to the Console
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << "[-v] serialport\n";
        return 1;
    }
    SafeQueue<Message> routerIn;
    SafeQueue<Message> serialIn;
    SafeQueue<Message> tunIn;
    SafeQueue<Message> consoleIn;
    bool verbose = false;
    unsigned opt = 1;
    if (argc >= 3 && argv[opt][0] == '-') {
        switch (argv[opt][1]) {
            case 'v':
                verbose = true;
                break;
            default:
                std::cout << "Ignoring uknown option \"" << argv[opt] << "\"\n";
        }
        ++opt;
    }
    std::cout << "Opening port " << argv[opt] << "\n";

    // rule 1: Everything from the Console goes to the serial port
    Console con(consoleIn, serialIn);
    // rule 2: Everything from the TUN goes to the serial port
    TunDevice tun(tunIn, serialIn);
    // rule 3: raw packets from the serial port go to the TUN
    // rule 4: non-raw packets from the serial port go to the Console
    Router rtr{routerIn, consoleIn, tunIn};
    SerialDevice ser{serialIn, routerIn, argv[opt], 115200};

    ser.verbosity(verbose);
    ser.hold();
    con.hold();
    tun.hold();
    rtr.hold();
    std::thread conThread{&Console::run, &con, &std::cin, &std::cout};
    std::thread serThread{&SerialDevice::run, &ser, &std::cin, &std::cout};
    std::thread tunThread{&TunDevice::run, &tun, &std::cin, &std::cout};
    std::thread rtrThread{&Router::run, &rtr, &std::cin, &std::cout};
    conThread.join();
    ser.releaseHold();
    serThread.join();  
    tun.releaseHold();
    tunThread.join();  
    rtr.releaseHold();
    rtrThread.join();  
}

