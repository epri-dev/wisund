/** 
 *  \file Device.cpp
 *  \brief Implementation of the Device class
 */
#include "Device.h"

Device::Device(SafeQueue<Message> &input, SafeQueue<Message> &output) :
        holdOnRxQueueEmpty{false},
        inQ{input},
        outQ{output}
{}
void Device::hold() 
{ 
    holdOnRxQueueEmpty = true; 
}
void Device::releaseHold() 
{ 
    holdOnRxQueueEmpty = false; 
    inQ.push(Message{nullptr, 0});
}

bool Device::wantHold() 
{ 
    return holdOnRxQueueEmpty; 
}

void Device::showHoldState() 
{ 
    std::cout << "State = " << std::boolalpha << holdOnRxQueueEmpty << "\n"; 
}

void Device::push(Message m) 
{ 
    outQ.push(m); 
}

void Device::wait_and_pop(Message &m) 
{ 
    inQ.wait_and_pop(m); 
}
