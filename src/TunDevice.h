#ifndef TUNDEVICE_H
#define TUNDEVICE_H

#include "Device.h"

class TunDevice : public Device
{
public:
    TunDevice(SafeQueue<Message> &input, SafeQueue<Message> &output);
    virtual ~TunDevice();
    int runTx(std::istream *in = &std::cin);
    int runRx(std::ostream *out = &std::cout);
    int run(std::istream *in, std::ostream *out);
private:
    void startReceive();
#if 0
    void handleMessage(const asio::error_code &error, std::size_t size);
#endif
    size_t send(const Message &msg);
    /// File descriptor for TUN device
    int fd;
};

#endif // TUNDEVICE_H
