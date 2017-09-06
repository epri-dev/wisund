#include "gpio.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#define VERBOSE 0

const std::string GPIO::basename{"/sys/class/gpio/"};
static const std::chrono::milliseconds briefly{50};

GPIO::GPIO(unsigned pinnum, bool out, bool state) :
    pin{pinnum}, name{basename+"gpio"+std::to_string(pin)+"/"}, dir{out}
{
    {
        std::ofstream out{basename + "export"};
	out.exceptions(out.failbit);
#if VERBOSE
	std::cout << "Exporting pin " << std::dec << pin << "\n";
#endif
	out << std::dec << pin;
    }
    /* Because it make take a little time to create
     * the associated file structure for an exported
     * pin, we wait until the file exists or we time
     * out.
     */
    std::this_thread::sleep_for(briefly);
    {
	std::ofstream out{name +  "direction"};
	out.exceptions(out.failbit);
#if VERBOSE
        std::cout << "Writing " << (dir ? "out" : "in") << " to " << name << "direction\n";
#endif
        out << (dir ? "out" : "in");
    }
    operator()(state);
}

GPIO::~GPIO() {
    std::ofstream out{basename + "unexport"};
    out.exceptions(out.failbit);
#if VERBOSE
        std::cout << "Unexporting pin " << std::dec << pin << "\n";
#endif
    out << std::dec << pin;
}

bool GPIO::operator()(bool on) {
    std::ofstream out{name + "value"};
    out.exceptions(out.failbit);
    out << (on ? "1" : "0");
#if VERBOSE
    std::cout << "Printing " << on << " to " << name << "value\n";
#endif
    return on;
}
