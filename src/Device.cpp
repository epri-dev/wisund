#include "Device.h"

Device::Device(SafeQueue<Message> &input, SafeQueue<Message> &output) :
        holdOnRxQueueEmpty{false},
        inQ{input},
        outQ{output}
    {}
void Device::push(Message m) { outQ.push(m); }
void Device::wait_and_pop(Message &m) { inQ.wait_and_pop(m); }
