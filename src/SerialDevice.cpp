#include "SerialDevice.h"
#include <thread>
#include <functional>
#include <iomanip>

SerialDevice::SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port, unsigned baud) :
    Device(input, output),
    serialport(port, baud)
{}
SerialDevice::~SerialDevice() = default;

int SerialDevice::runTx(std::istream *in)
{
    in = in;
    if (wantHold()) {
        startReceive(); 
    }
    return 0;
}

int SerialDevice::runRx(std::ostream *out)
{
    out = out;
    Message m{0};
    while (wantHold()) {
        wait_and_pop(m);
        serialport.send(m);
    }
    serialport.m_port.close();
    return 0;
}
int SerialDevice::run(std::istream *in, std::ostream *out)
{
    std::thread t1{&SerialDevice::runRx, this, out};
    int status = runTx(in);
    serialport.m_io.run();
    t1.join();
    return status;
}

using iterator = asio::buffers_iterator<asio::streambuf::const_buffers_type>;

std::pair<iterator, bool> match_slip(iterator begin, iterator end) {
    // find the first
    iterator first = begin;
    for ( ; first != end && static_cast<uint8_t>(*first) != Serial::END; ++first) 
    { /* just find the first */ }
    if (first == end) {
        return std::make_pair(begin, false);
    }
    // found first; now find second
    iterator last = first;
    for ( ++last ; last != end; ++last) {
        if (static_cast<uint8_t>(*last) == Serial::END) {
            return std::make_pair(++last, true);
        }
    }
    return std::make_pair(begin, false);
}

void SerialDevice::startReceive() {
    asio::async_read_until(serialport.m_port, 
        serialport.m_data, 
        match_slip,
        std::bind(&SerialDevice::handleMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void SerialDevice::handleMessage(const::asio::error_code &error, std::size_t size) {
    if (error) {
        return;
    }

    auto buf = serialport.m_data.data();
    std::vector<uint8_t> v(size);
    asio::buffer_copy(asio::buffer(v), buf);
    Message m{v};
    push(Serial::decode(m));
    serialport.m_data.consume(size);
    if (wantHold())
        startReceive();
}

