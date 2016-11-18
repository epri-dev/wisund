#include "Device.h"

Device::Device(SafeQueue<Message> &input, SafeQueue<Message> &output) :
        in{input},
        out{output}
    {}
void Device::push(Message m) { out.push(m); }
