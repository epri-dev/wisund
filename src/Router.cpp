#include <iostream>
#include "Serial.h"
#include "SafeQueue.h"
#include "Console.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " serialport\n";
        return 1;
    }
    SafeQueue<Message> routerIn;
    SafeQueue<Message> serialIn;
    SafeQueue<Message> consoleIn;

    std::cout << "Opening port " << argv[1] << "\n";
    Serial s{argv[1], 115200};
    Console con(consoleIn, routerIn);
    return con.run();
}

