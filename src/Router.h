#ifndef ROUTER_H
#define ROUTER_H
/** 
 *  \file Router.h
 *  \brief Interface for the Router class
 */

#include "Device.h"

/** 
 * \brief class for router device.
 *
 * This class receives Messages from its single input queue, but unlike 
 * other Devices, the Router has two output queues; one for raw (IPv6)
 * Messages and the other for all other Messages.  The purpose of the 
 * Router is solely to classify incoming Messages and to place them in
 * the appropriate output queue.
 *
 */
class Router : public Device 
{
public:
    /// constructor takes one input and two output queues 
    Router(SafeQueue<Message> &input, SafeQueue<Message> &output, SafeQueue<Message> &rawOutput);
    /// destructor is virtual in case class needs to be further derived
    virtual ~Router();
    /// runs both the receive and transmit handlers in required sequence
    int run(std::istream *in, std::ostream *out);
    /// set or clear verbose flag and return previous state
    bool verbosity(bool verbose);
private:
    /// this queue is for raw (IPv6) messages
    SafeQueue<Message> &rawQ;
    /// if true, provide more diagnostic output
    bool m_verbose;
};

#endif // ROUTER_H
