#ifndef REPLY_H
#define REPLY_H
#include "Message.h"
#include <ostream>
/// standalone function that parses reply message to passed ostream
void decode(const Message &msg, std::ostream &out);
/// standalone function to parse reply message into string
std::string decode(const Message &msg);
#endif // REPLY_H
