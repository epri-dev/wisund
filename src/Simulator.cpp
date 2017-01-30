#include "Simulator.h"
#include <thread>

/// constructor takes references to input and output queues, serial port and baud rate
Simulator::Simulator(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    m_verbose{false},
    m_delay{0},
    lastMessage{}
{}

/// destructor is virtual in case class needs to be further derived
Simulator::~Simulator() = default;

/// runs the transmit handler (wrapping messages in SLIP encapsulation before sending)
int Simulator::runTx(std::istream *in)
{
    in = in;
    if (wantHold()) {
        receive(); 
    }
    return 0;
}

/// runs the receive handler (unwrapping messages in SLIP encapsulation)
int Simulator::runRx(std::ostream *out)
{
    out = out;
    Message m{};
    while (wantHold()) {
        wait_and_pop(m);
        send(m);
    }
    return 0;
}

/// runs both the receive and transmit handlers in required sequence
int Simulator::run(std::istream *in, std::ostream *out)
{
    std::thread t1{&Simulator::runRx, this, out};
    int status = runTx(in);
    t1.join();
    return status;
}
//
/// set or clear verbose flag and return previous state
bool Simulator::verbosity(bool verbose)
{
    std::swap(verbose, m_verbose);
    return verbose;
}

/// set optional pre-send delay time 
void Simulator::sendDelay(std::chrono::duration<float, std::milli> delay) 
{
    m_delay = delay;
}

/// encodes and sends a message
size_t Simulator::send(const Message &msg) 
{
    if (msg.size() == 0) {
        return 0;
    }
    if (m_verbose) {
        std::cout << "sending: " << msg << "\n";
    }
    std::this_thread::sleep_for(m_delay);
    lastMessage = msg;
    return msg.size();
}

/// receive simulated message from simulated device
void Simulator::receive() 
{
    // TODO: create receive message based on last transmitted msg
}
