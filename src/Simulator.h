#ifndef SIMULATOR_H
#define SIMULATOR_H
/** 
 *  \file Simulator.h
 *  \brief Interface for the Simulator class
 */

#include "Device.h"

/**
 * \brief wrapper class for the simulator.
 *
 * This class uses its Message queues to send and receive data via the
 * a simulated radio interface.
 */
class Simulator : public Device
{
public:
    /// constructor takes references to input and output queues, serial port and baud rate
    Simulator(SafeQueue<Message> &input, SafeQueue<Message> &output);
    /// destructor is virtual in case class needs to be further derived
    virtual ~Simulator();
    /// runs both the receive and transmit handlers in required sequence
    int run(std::istream *in, std::ostream *out);
    /// set or clear verbose flag and return previous state
    bool verbosity(bool verbose);
    /// set optional pre-send delay time 
    void sendDelay(std::chrono::duration<float, std::milli> delay); 
private:
    /// start receiving the message
    void receive();
    /// encodes and sends a message
    size_t send(const Message &msg);

    /// if true, echo raw packets
    bool m_verbose;
    /// delay before each packet is sent
    std::chrono::duration<float, std::milli> m_delay;
    /// last received message
    Message lastMessage;
};

#endif // SIMULATOR_H
