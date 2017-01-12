#ifndef MESSAGE_H
#define MESSAGE_H
#include <cstdint>
#include <vector>
#include <initializer_list>
#include <iostream>

/**
 * A Message is a specialization of a vector of bytes.
 */
class Message : public std::vector<uint8_t> {
public:
    /// create a Message from an initializer list
    Message(std::initializer_list<uint8_t> b); 
    /// create a Message from a raw pointer and passed size
    Message(const uint8_t *msg, size_t size);
    /// create a Message from a vector
    Message(std::vector<uint8_t> v);
    /// create a Message by moving from a vector
    Message(std::vector<uint8_t> &&v);
    /// append another message to this one
    Message &operator+=(const Message& msg);
    /// returns true if the vector is raw (that is, begins with a 0x00 byte)
    bool isRaw() const;
    /// overloaded inserter dumps the Message as a sequence of hex bytes
    friend std::ostream& operator<<(std::ostream &out, const Message &msg);
};
#endif // MESSAGE_H
