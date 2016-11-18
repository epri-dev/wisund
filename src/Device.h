#ifndef DEVICE_H
#define DEVICE_H
#include "Message.h"
#include "SafeQueue.h"

class Device {
public:
    Device(SafeQueue<Message> &input, SafeQueue<Message> &output);
    virtual void push(Message m);
protected:
    SafeQueue<Message> &in;
    SafeQueue<Message> &out;
};

#endif // DEVICE_H
