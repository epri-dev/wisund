#include "SerialDevice.h"
#include <thread>
#include <functional>
#include <iomanip>

static constexpr uint8_t END{0xc0};
static constexpr uint8_t ESC{0xdb};
static constexpr uint8_t ESC_END{0xdc}; 
static constexpr uint8_t ESC_ESC{0xdd};

SerialDevice::SerialDevice(SafeQueue<Message> &input, SafeQueue<Message> &output, const char *port, unsigned baud) :
    Device(input, output),
    m_io(), 
    m_port(m_io, port),
    m_verbose{false}
{
    m_port.set_option(asio::serial_port_base::baud_rate(baud));
}

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
        send(m);
    }
    m_port.close();
    return 0;
}

int SerialDevice::run(std::istream *in, std::ostream *out)
{
    std::thread t1{&SerialDevice::runRx, this, out};
    int status = runTx(in);
    m_io.run();
    t1.join();
    return status;
}

using iterator = asio::buffers_iterator<asio::streambuf::const_buffers_type>;

static std::pair<iterator, bool> match_slip(iterator begin, iterator end) {
    // find the first
    iterator first = begin;
    for ( ; first != end && static_cast<uint8_t>(*first) != END; ++first) 
    { /* just find the first */ }
    if (first == end) {
        return std::make_pair(begin, false);
    }
    // found first; now find second
    iterator last = first;
    for ( ++last ; last != end; ++last) {
        if (static_cast<uint8_t>(*last) == END) {
            return std::make_pair(++last, true);
        }
    }
    return std::make_pair(begin, false);
}

void SerialDevice::startReceive() {
    asio::async_read_until(m_port, 
        m_data, 
        match_slip,
        std::bind(&SerialDevice::handleMessage, this, std::placeholders::_1, std::placeholders::_2));
}

void SerialDevice::handleMessage(const::asio::error_code &error, std::size_t size) {
    if (error) {
        return;
    }
    auto buf = m_data.data();
    std::vector<uint8_t> v(size);
    asio::buffer_copy(asio::buffer(v), buf);
    Message msg{v};
    if (m_verbose) {
        std::cout << "received: " << msg << "\n";
    }
    push(SerialDevice::decode(msg));
    m_data.consume(size);
    if (wantHold()) {
        startReceive();
    }
}

size_t SerialDevice::send(const Message &msg) {
    auto encoded = encode(msg);
    if (m_verbose) {
        std::cout << "sending: " << encoded << "\n";
    }
    return m_port.write_some(asio::buffer(encoded.data(), encoded.size()));
}

bool SerialDevice::verbosity(bool verbose) {
    std::swap(verbose, m_verbose);
    return verbose;
}

Message SerialDevice::encode(const Message &msg) {
    // wrap the payload inside 0xC0 ... 0xC0 
    Message ret{END};
    for (const auto &byte : msg) {
        switch (byte) {
        case END:
            ret.push_back(ESC);
            ret.push_back(ESC_END);
            break;
        case ESC:
            ret.push_back(ESC);
            ret.push_back(ESC_ESC);
            break;
        default:
            ret.push_back(byte);
        }
    }
    ret.push_back(END);
    return ret;
}

Message SerialDevice::decode(const Message &msg) {
    std::vector<uint8_t> ret; 
    uint8_t prev = msg.front();
    for (auto it = msg.begin(); it != msg.end(); ) {
        switch(*it) {
            case END:
            case ESC:
                // do nothing
                break;
            case ESC_END:
                ret.push_back(prev == ESC ? END : ESC_END);
                break;
            case ESC_ESC:
                ret.push_back(prev == ESC ? ESC : ESC_ESC);
                break;
            default:
                ret.push_back(*it);
        }
        prev = *it++;
    }
    return Message{ret};
}
