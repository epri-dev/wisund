#ifndef DEVICE_H
#define DEVICE_H
#include "Message.h"
#include "SafeQueue.h"
#include <atomic>

class Device {
public:
    Device(SafeQueue<Message> &input, SafeQueue<Message> &output);
    virtual void push(Message m);
    virtual void wait_and_pop(Message &m);
    virtual bool more() { return !inQ.empty(); }
    virtual int run(std::istream *in, std::ostream *out) = 0;
    void hold();
    void releaseHold();
    bool wantHold();
    void showHoldState();
protected:
    volatile std::atomic_bool holdOnRxQueueEmpty;
    SafeQueue<Message> &inQ;
    SafeQueue<Message> &outQ;
};

#endif // DEVICE_H
