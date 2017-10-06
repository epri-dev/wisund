#ifndef CAPTUREDEVICE_H
#define CAPTUREDEVICE_H
/** 
 *  \file CaptureDevice.h
 *  \brief Interface for the CaptureDevice class
 */

#include "SinkDevice.h"

/** 
 * \brief class for capture device.
 *
 * This class receives Messages from its single input queue, but unlike 
 * other Devices, the CaptureDevice has no output queues.
 *
 */
class CaptureDevice : public SinkDevice 
{
public:
    /// constructor takes one input queue 
    CaptureDevice(SafeQueue<Message> &input);
    /// destructor is virtual in case class needs to be further derived
    virtual ~CaptureDevice();
    /// runs both the receive and transmit handlers in required sequence
    int run(std::istream *in, std::ostream *out);
    /// set or clear verbose flag and return previous state
    bool verbosity(bool verbose);
private:
    /// if true, provide more diagnostic output
    bool m_verbose;
};

#endif // CAPTUREDEVICE_H
