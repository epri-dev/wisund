#include <iostream>
#include <thread>
#include "SafeQueue.h"
#include "Console.h"
#include "SerialDevice.h"

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
    SafeQueue<Message> consoleIn;
    bool verbose = false;
    unsigned opt = 1;
    if (argc >= 3 && argv[opt][0] == '-') {
        switch (argv[opt][1]) {
            case 'v':
                verbose = true;
            default:
                std::cout << "Ignoring uknown option \"" << argv[opt] << "\"\n";
        }
        ++opt;
    }
    std::cout << "Opening port " << argv[opt] << "\n";

    // rule 1
    Console con(consoleIn, serialIn);
    // rule 2
    // not done yet
    // rule 3
    SerialDevice ser{serialIn, consoleIn, argv[opt], 115200};
    ser.verbosity(verbose);
    ser.hold();
    con.hold();
    std::thread conThread{&Console::run, &con, &std::cin, &std::cout};
    std::thread serThread{&SerialDevice::run, &ser, &std::cin, &std::cout};
    conThread.join();
    ser.releaseHold();
    serThread.join();  
}

