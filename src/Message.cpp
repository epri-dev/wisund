#include "Message.h"
#include <iostream>
#include <iomanip>
#include <initializer_list>

#include <utility> 

Message::Message(std::initializer_list<uint8_t> b) 
    : std::vector<uint8_t>(b)
{}
Message::Message(uint8_t *msg, size_t size) 
    : std::vector<uint8_t>(size,0) 
    {
        for (size_t i=0; i < size; ++i) {
            push_back(msg[i]);
        }
    }
Message::Message(std::vector<uint8_t> v) 
    : std::vector<uint8_t>(v)
    {
    }

bool Message::isRaw() const { return size() && front() == 0; }

// give SLIP characters names
enum { END = 0xc0, ESC = 0xdb, ESC_END = 0xdc, ESC_ESC = 0xdd };

Message Message::encode() const {
    // wrap the payload inside 0xC0 ... 0xC0 
    Message ret{END};
    for (const auto &byte : *this) {
        switch (byte) {
        case END:
            ret.push_back(ESC);
            ret.push_back(ESC_END);
            break;
        case ESC:
            ret.push_back( ESC);
            ret.push_back( ESC_ESC);
            break;
        default:
            ret.push_back(byte);
        }
    }
    ret.push_back(END);
    return ret;
}

Message Message::decode() const {
    std::vector<uint8_t> ret; 
    uint8_t prev = front();
    for (auto it = begin(); it != end(); ) {
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

std::ostream& operator<<(std::ostream &out, const Message &msg) {
    for (auto byte : msg) {
        out << std::setw(2) << std::hex << std::setfill('0') 
            << static_cast<unsigned>(byte);
    }
    return out;
}

#if 0
int main() {
    Message m{0x06, 0x21, 1};
    std::cout << m << "\n";
    std::cout << (m.isRaw() ? "plain" : "not plain") << "\n";
}
#endif
