#ifndef DEVICE_H
#define DEVICE_H
#include "Message.h"
#include "SafeQueue.h"

class Device {
public:
    Device(SafeQueue<Message> &input, SafeQueue<Message> &output);
    virtual void push(Message m);
    virtual void wait_and_pop(Message &m);
    virtual bool more() { return !inQ.empty(); }
    virtual int run(std::istream *in, std::ostream *out) = 0;
    void stopRx() { holdOnRxQueueEmpty = false; }
    void holdRx() { holdOnRxQueueEmpty = true; }
    void showState() { std::cout << "State = " << std::boolalpha << holdOnRxQueueEmpty << "\n"; }
protected:
    volatile bool holdOnRxQueueEmpty;
    SafeQueue<Message> &inQ;
    SafeQueue<Message> &outQ;
};

#endif // DEVICE_H
