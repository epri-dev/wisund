#ifndef SINKDEVICE_H
#define SINKDEVICE_H
/** 
 *  \file SinkDevice.h
 *  \brief Interface for the SinkDevice class
 */

#include "Message.h"
#include "SafeQueue.h"
#include <atomic>

/**
 * \brief This is the base class for all devices that pass Messages.
 *
 */
class SinkDevice {
public:
    /// construct with reference for input queues
    SinkDevice(SafeQueue<Message> &input);
    /// wait for a message to appear in the input queue and pop it
    virtual void wait_and_pop(Message &m);
    /// returns true if the input queue is not empty
    virtual bool more() { return !inQ.empty(); }
    /// runs both receive and transmit processing (which could run in different threads)
    virtual int run(std::istream *in, std::ostream *out) = 0;
    /// causes the device to hold (keep running) even if the input queue is empty
    void hold();
    /// releases any hold that may have been asserted on this SinkDevice
    void releaseHold();
    /// returns the current hold status of this SinkDevice
    bool wantHold();
    /// convenience function to print the state of the hold variable to `std::cout`
    void showHoldState();
protected:
    /// If true, the receive will continue even if the input queue is empty
    volatile std::atomic_bool holdOnRxQueueEmpty;
    /// input message queue for this device
    SafeQueue<Message> &inQ;
};

#endif // SINKDEVICE_H
