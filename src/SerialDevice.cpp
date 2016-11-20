#include "SerialDevice.h"
#include <thread>


SerialDevice::SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port, unsigned baud) :
    Device(input, output),
    serialport(port, baud)
{}
SerialDevice::~SerialDevice() = default;

int SerialDevice::runTx(std::istream *in)
{
    while (wantHold()) {
        Message m = serialport.receive();
        std::cout << "SerialDevice is pushing " << m << '\n';
        push(m);
    }
    return 0;
}
int SerialDevice::runRx(std::ostream *out)
{
    Message m{0};
    while (wantHold()) {
        *out << "SerialDevice waiting for message\n";
        wait_and_pop(m);
        *out << "SerialDevice received message: " << m << '\n';
        serialport.send(m);
    }
    *out << "SerialDevice::runRx ending\n";
    return 0;
}
int SerialDevice::run(std::istream *in, std::ostream *out)
{
    std::thread t1{&SerialDevice::runTx, this, in};
    int status = runRx(out);
    t1.join();
    return status;
}
