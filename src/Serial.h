#ifndef SERIAL_H
#define SERIAL_H
#include <asio.hpp>
#include "Message.h"

class Serial {
public:
    Serial(const char *port, unsigned baud);
    ~Serial();
    size_t receive(void *data, size_t length);
    size_t send(void *data, size_t length);
    size_t send(const Message &msg);
    Message receive();
    void close();

private:
    static Message encode(const Message &msg);
    static Message decode(const Message &msg);

    uint8_t read_msg_[1024];
    asio::io_service m_io;
    asio::serial_port m_port;
};
#endif // SERIAL_H
