#ifndef REPLY_H
#define REPLY_H
#include "Message.h"
#include <ostream>
/// standalone function that parses reply message to passed ostream
void decode(const Message &msg, std::ostream &out);
#endif // REPLY_H
