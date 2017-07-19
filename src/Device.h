#ifndef DEVICE_H
#define DEVICE_H
/** 
 *  \file Device.h
 *  \brief Interface for the Device class
 */

#include "Message.h"
#include "SafeQueue.h"
#include <atomic>

/**
 * \brief This is the base class for all devices that pass Messages.
 *
 */
class Device {
public:
    /// construct with references for input and output queues
    Device(SafeQueue<Message> &input, SafeQueue<Message> &output);
    /// push a message to the output queue
    virtual void push(Message m);
    /// wait for a message to appear in the input queue and pop it
    virtual void wait_and_pop(Message &m);
    /// returns true if the input queue is not empty
    virtual bool more() { return !inQ.empty(); }
    /// runs both receive and transmit processing (which could run in different threads)
    virtual int run(std::istream *in, std::ostream *out) = 0;
    /// causes the device to hold (keep running) even if the input queue is empty
    void hold();
    /// releases any hold that may have been asserted on this Device
    void releaseHold();
    /// returns the current hold status of this Device
    bool wantHold();
    /// convenience function to print the state of the hold variable to `std::cout`
    void showHoldState();
protected:
    /// If true, the receive will continue even if the input queue is empty
    volatile std::atomic_bool holdOnRxQueueEmpty;
    /// input message queue for this device
    SafeQueue<Message> &inQ;
    /// output message queue for this device
    SafeQueue<Message> &outQ;
};

#endif // DEVICE_H
