#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H

#include "Device.h"
#include <asio.hpp>

class SerialDevice : public Device
{
public:
    SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port = "/dev/ttyACM0", unsigned baud=115200);
    virtual ~SerialDevice();
    int runTx(std::istream *in = &std::cin);
    int runRx(std::ostream *out = &std::cout);
    int run(std::istream *in, std::ostream *out);
    static Message encode(const Message &msg);
    static Message decode(const Message &msg);
private:
    void startReceive();
    void handleMessage(const asio::error_code &error, std::size_t size);
    size_t send(const Message &msg);

    asio::io_service m_io;
    asio::serial_port m_port;
    asio::streambuf m_data;
};

#endif // SERIALDEVICE_H

