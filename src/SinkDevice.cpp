/** 
 *  \file SinkDevice.cpp
 *  \brief Implementation of the SinkDevice class
 */
#include "SinkDevice.h"

SinkDevice::SinkDevice(SafeQueue<Message> &input) :
        holdOnRxQueueEmpty{false},
        inQ{input}
{}
void SinkDevice::hold() 
{ 
    holdOnRxQueueEmpty = true; 
}
void SinkDevice::releaseHold() 
{ 
    holdOnRxQueueEmpty = false; 
    inQ.push(Message{nullptr, 0});
}

bool SinkDevice::wantHold() 
{ 
    return holdOnRxQueueEmpty; 
}

void SinkDevice::showHoldState() 
{ 
    std::cout << "State = " << std::boolalpha << holdOnRxQueueEmpty << "\n"; 
}

void SinkDevice::wait_and_pop(Message &m) 
{ 
    inQ.wait_and_pop(m); 
}
