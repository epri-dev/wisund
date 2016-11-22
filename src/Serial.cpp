#include "Serial.h"
#include <algorithm>
#include <istream>

const uint8_t Serial::END{0xc0};
const uint8_t Serial::ESC{0xdb};
const uint8_t Serial::ESC_END{0xdc}; 
const uint8_t Serial::ESC_ESC{0xdd};

Serial::Serial(const char *port, unsigned baud) 
    : m_io(), 
      m_port(m_io, port)
{
    m_port.set_option(asio::serial_port_base::baud_rate(baud));
}
Serial::~Serial() {
    m_port.close();
}
size_t Serial::receive(void *data, size_t length) {
    return m_port.read_some(asio::buffer(data, length));
}
size_t Serial::send(void *data, size_t length) {
    return m_port.write_some(asio::buffer(data, length));
}
size_t Serial::send(const Message &msg) {
    auto encoded = encode(msg);
    return m_port.write_some(asio::buffer(encoded.data(), encoded.size()));
}

Message Serial::encode(const Message &msg) {
    // wrap the payload inside 0xC0 ... 0xC0 
    Message ret{END};
    for (const auto &byte : msg) {
        switch (byte) {
        case END:
            ret.push_back(ESC);
            ret.push_back(ESC_END);
            break;
        case ESC:
            ret.push_back(ESC);
            ret.push_back(ESC_ESC);
            break;
        default:
            ret.push_back(byte);
        }
    }
    ret.push_back(END);
    return ret;
}

Message Serial::decode(const Message &msg) {
    std::vector<uint8_t> ret; 
    uint8_t prev = msg.front();
    for (auto it = msg.begin(); it != msg.end(); ) {
        switch(*it) {
            case END:
                // do nothing
                break;
            case ESC_END:
                if (prev == ESC) {
                    ++it;
                    ret.push_back(END);
                }
                break;
            case ESC_ESC:
                if (prev == ESC) {
                    ++it;
                    ret.push_back(ESC);
                }
                break;
            default:
                ret.push_back(*it);
        }
        prev = *it++;
    }
    return Message{ret};
}
