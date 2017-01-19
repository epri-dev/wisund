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
    bool strict(bool strict);
    bool verbosity(bool verbose);
private:
    void startReceive();
#if 0
    void handleMessage(const asio::error_code &error, std::size_t size);
#endif
    size_t send(const Message &msg);
    bool isCompleteIpV6Msg(const Message& msg) const;
    /// File descriptor for TUN device
    int fd;
    bool m_verbose;
    bool m_ipv6only;
};

#endif // TUNDEVICE_H
