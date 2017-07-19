#ifndef SERIALDEVICE_H
#define SERIALDEVICE_H
/** 
 *  \file SerialDevice.h
 *  \brief Interface for the SerialDevice class
 */


#include "Device.h"
#include <asio.hpp>

/**
 * \brief wrapper class for the serial port.
 *
 * This class uses its Message queues to send and receive data via the
 * serial port to the radio.
 */
class SerialDevice : public Device
{
public:
    /// constructor takes references to input and output queues, serial port and baud rate
    SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port = "/dev/ttyACM0", unsigned baud=115200);
    /// destructor is virtual in case class needs to be further derived
    virtual ~SerialDevice();
    /// runs the transmit handler (wrapping messages in SLIP encapsulation before sending)
    int runTx(std::istream *in = &std::cin);
    /// runs the receive handler (unwrapping messages in SLIP encapsulation)
    int runRx(std::ostream *out = &std::cout);
    /// runs both the receive and transmit handlers in required sequence
    int run(std::istream *in, std::ostream *out);
    /// set or clear verbose flag and return previous state
    bool verbosity(bool verbose);
    /// set or clear rawpacket flag and return previous state
    bool setraw(bool rawpackets);
    /// set optional pre-send delay time 
    void sendDelay(std::chrono::duration<float, std::milli> delay); 
    /// encapsulate the message using SLIP coding
    static Message encode(const Message &msg);
    /// decapsulate the message using SLIP coding
    static Message decode(const Message &msg);
private:
    /// start receiving the message
    void startReceive();
    /// callback handler to finish receiving and decapsulating the message
    void handleMessage(const asio::error_code &error, std::size_t size);
    /// encodes and sends a message
    size_t send(const Message &msg);

    /// ASIO IO service object
    asio::io_service m_io;
    /// the serial port
    asio::serial_port m_port;
    /// a stream buffer used by the receive functions
    asio::streambuf m_data;
    /// if true, echo encoded packets
    bool m_verbose;
    /// if true, and if m_verbose, echo unencoded packets
    bool m_raw;
    /// delay before each packet is sent
    std::chrono::duration<float, std::milli> m_delay;
};

#endif // SERIALDEVICE_H

