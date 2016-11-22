#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include "Serial.h"
#include "Device.h"

class SerialDevice : public Device
{
public:
    SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port = "/dev/ttyACM0", unsigned baud=115200);
    virtual ~SerialDevice();
    int runTx(std::istream *in = &std::cin);
    int runRx(std::ostream *out = &std::cout);
    int run(std::istream *in, std::ostream *out);
private:
    void startReceive();
    void handleMessage(const asio::error_code &error, std::size_t size);
    Serial serialport;
};

#endif // SERIALDEVICE_H

