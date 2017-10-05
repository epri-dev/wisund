/** 
 *  \file wisund.cpp
 *  \brief implemenation of WiSUN "daemon"
 */
#include <asio.hpp>
#include <iostream>
#include <thread>
#include <chrono>
#include "SafeQueue.h"
#include "Console.h"
#include "Router.h"
#include "SerialDevice.h"
#include "TunDevice.h"
#include "CaptureDevice.h"

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
 *    4. If a capture packet comes from the serial port, it goes to the Capture device
 *    5. All non-raw, non-capture packets from the serial port goes to the Console
 */
int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " [-e] [-v] [-r] [-d msdelay] [-s] serialport\n";
        return 1;
    }
    SafeQueue<Message> routerIn;
    SafeQueue<Message> serialIn;
    SafeQueue<Message> tunIn;
    SafeQueue<Message> consoleIn;
    SafeQueue<Message> captureIn;
    bool verbose = false;
    bool strict = false;
    bool rawpackets = false;
    bool echo = false;
    std::chrono::milliseconds delay{0};
    int opt = 1;
    while (opt < argc && argv[opt][0] == '-') {
        switch (argv[opt][1]) {
            case 'v':
                verbose = true;
                break;
            case 'r':
                rawpackets = true;
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
    if (opt >= argc) {
        std::cout << "Error: no device given\n";
        return 0;
    }
    std::cout << "Opening port " << argv[opt] << "\n";

    // rule 1: Everything from the Console goes to the serial port
    Console con(consoleIn, serialIn);
    // rule 2: Everything from the TUN goes to the serial port
    TunDevice tun(tunIn, serialIn);
    tun.strict(strict);
    // rule 3: raw packets from the serial port go to the TUN
    // rule 4: If a capture packet comes from the serial port, it goes to the Capture device
    // rule 5: All non-raw, non-capture packets from the serial port goes to the Console
    Router rtr{routerIn, consoleIn, tunIn, captureIn};
    SerialDevice ser{serialIn, routerIn, argv[opt], 115200};
    CaptureDevice cap{captureIn};

    ser.sendDelay(delay);
    ser.verbosity(verbose);
    ser.setraw(rawpackets);
    con.setEcho(echo);
    ser.hold();
    tun.hold();
    rtr.hold();
    cap.hold();

    std::thread serThread{&SerialDevice::run, &ser, &std::cin, &std::cout};
    std::thread tunThread{&TunDevice::run, &tun, &std::cin, &std::cout};
    std::thread rtrThread{&Router::run, &rtr, &std::cin, &std::cout};
    std::thread capThread{&CaptureDevice::run, &cap, &std::cin, &std::cout};

    asio::io_service ios;
    // standard HTTP port, IPv4 address
    asio::ip::tcp::acceptor acceptor(ios, 
            asio::ip::tcp::endpoint{asio::ip::tcp::v4(), 5555});
    while (!con.getQuitValue()) {
        con.hold();
        asio::ip::tcp::iostream stream;
        acceptor.accept(*stream.rdbuf());
        con.run(&stream, &stream);
        if (con.wantReset()) {
            std::cout << "resetting\n";
        }
    }

    ser.releaseHold();
    serThread.join();  
    tun.releaseHold();
    tunThread.join();  
    rtr.releaseHold();
    rtrThread.join();  
    cap.releaseHold();
    capThread.join();  
}

