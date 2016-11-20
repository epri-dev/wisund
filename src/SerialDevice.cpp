#include "SerialDevice.h"
#include <thread>


SerialDevice::SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port, unsigned baud) :
    Device(input, output),
    serialport(port, baud)
{}
SerialDevice::~SerialDevice() = default;

int SerialDevice::runTx(std::istream *in)
{
    while (holdOnRxQueueEmpty) {
        Message m = serialport.receive();
        std::cout << "SerialDevice is pushing " << m << '\n';
        push(m);
    }
    return 0;
}
int SerialDevice::runRx(std::ostream *out)
{
    Message m{0};
    while (holdOnRxQueueEmpty) {
        wait_and_pop(m);
        serialport.send(m);
    }
    serialport.close();
    return 0;
}
int SerialDevice::run(std::istream *in, std::ostream *out)
{
    std::thread t1{&SerialDevice::runRx, this, out};
    int status = runTx(in);
    t1.join();
    return status;
}
