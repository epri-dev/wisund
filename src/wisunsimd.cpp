#include <asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "SafeQueue.h"
#include "Console.h"
#include "Simulator.h"

/*
 * The simulator works the same way as the wisund code except that 
 * instead of using a real serial port and a real TUN device, both are 
 * simulated. Because the TUN device for the real winsund code only
 * has indirect effects, it is omitted entirely from this simulator
 * and only the radio messages are simulated.
 * 
 * For that reason, no Router is required, since the Console and
 * Simulator devices are the two devices.  Messages originating 
 * from one simply always go to the other.
 *
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [-e] [-v] [-r] [-d msdelay] [-s] serialport\n";
        return 1;
    }
    SafeQueue<Message> serialIn;
    SafeQueue<Message> consoleIn;
    bool verbose = false;
    bool strict = false;
    // bool rawpackets = false;
    bool echo = false;
    std::chrono::milliseconds delay{0};
    int opt = 1;
    while (opt < argc && argv[opt][0] == '-') {
        switch (argv[opt][1]) {
            case 'v':
                verbose = true;
                break;
            case 'r':
                // we only do raw packets with the simulator, so silently igore
                // rawpackets = true;
                break;
            case 's':
                strict = true;
                break;
            case 'e':
                echo = true;
                break;
            case 'd':
                // TODO: error handling if next arg is not a number
                delay = std::chrono::milliseconds{std::atoi(argv[++opt])};
                break;
            default:
                std::cout << "Ignoring uknown option \"" << argv[opt] << "\"\n";
        }
        ++opt;
    }
    strict = strict;
    if (opt >= argc) {
        std::cout << "Error: no device given\n";
        return 0;
    }
    std::cout << "Opening port " << argv[opt] << "\n";

    // rule 1: Everything from the Console goes to the serial port
    Console con(consoleIn, serialIn);
    // rule 2: Everything from serial port goes to the console
    Simulator ser(serialIn, consoleIn);

    ser.sendDelay(delay);
    ser.verbosity(verbose);
    //ser.setraw(rawpackets);
    con.setEcho(echo);
    ser.hold();

    std::thread serThread{&Simulator::run, &ser, &std::cin, &std::cout};

    asio::io_service ios;
    // standard HTTP port, IPv4 address
    asio::ip::tcp::acceptor acceptor(ios, 
            asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 5555});
    while (!con.getQuitValue()) {
        con.hold();
        asio::ip::tcp::iostream stream;
        acceptor.accept(*stream.rdbuf());
        con.run(&stream, &stream);
    }

    ser.releaseHold();
    serThread.join();  
}
