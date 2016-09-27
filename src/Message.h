#ifndef MESSAGE_H
#define MESSAGE_H
#include <cstdint>
#include <vector>
#include <initializer_list>
#include <iostream>

class Message : public std::vector<uint8_t> {
public:
    Message(std::initializer_list<uint8_t> b); 
    Message(uint8_t *msg, size_t size);
    Message(std::vector<uint8_t> v);
    bool isRaw() const;
    Message encode() const;
    Message decode() const;
    friend std::ostream& operator<<(std::ostream &out, const Message &msg);
friend std::ostream& operator<<(std::ostream &out, const Message &msg); 
};
#endif // MESSAGE_H
