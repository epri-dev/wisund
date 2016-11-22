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

    // give SLIP characters names
    static const uint8_t END;
    static const uint8_t ESC;
    static const uint8_t ESC_END; 
    static const uint8_t ESC_ESC;

public:
    static Message encode(const Message &msg);
    static Message decode(const Message &msg);

    uint8_t read_msg_[1024];
    asio::io_service m_io;
    asio::serial_port m_port;
    asio::streambuf m_data;
};
#endif // SERIAL_H
