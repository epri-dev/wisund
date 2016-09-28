#include "Serial.h"
#include <algorithm>
#include <istream>

Serial::Serial(const char *port, unsigned baud) 
    : m_io(), 
      m_port(m_io, port)
{
    m_port.set_option(asio::serial_port_base::baud_rate(baud));
}
Serial::~Serial() {
    m_port.close();
}
size_t Serial::receive(void *data, size_t length) {
    return m_port.read_some(asio::buffer(data, length));
}
size_t Serial::send(void *data, size_t length) {
    return m_port.write_some(asio::buffer(data, length));
}
size_t Serial::send(const Message &msg) {
    return m_port.write_some(asio::buffer(msg.data(), msg.size()));
}
Message Serial::receive() {
    std::vector<uint8_t> mutableBuffer(1024);
    std::vector<uint8_t> msg;
    asio::streambuf b;
    size_t sz = read_until(m_port, b, '\xc0');
    msg.push_back('\xc0');
    asio::streambuf c;
    sz += read_until(m_port, c, '\xc0');
    std::cout << "Read " << std::dec << sz << " bytes\n";
    std::istream is(&c);
    std::copy(std::istream_iterator<uint8_t>(is), 
            std::istream_iterator<uint8_t>(),
            back_inserter(msg));
    return Message{msg};
}
