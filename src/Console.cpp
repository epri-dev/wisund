#include "Console.h"
#include "scanner.h"
#include <thread>
#include <iterator>
#include <iomanip>
#include <sstream>
#include <vector>


Console::Console(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    trace_scanning{false},
    trace_parsing{false}
{}

Console::~Console() = default;

int Console::runTx(std::istream *in) {
    Scanner scanner(in);
    yy::Parser parser(scanner, *this);
    parser.set_debug_level(trace_parsing);
    int status = parser.parse();
    releaseHold();
    return status;
}

int Console::runRx(std::ostream *out) {
    Message m{0};
    while (wantHold() || more()) {
        wait_and_pop(m);
        decode(m, *out);
    }
    return 0;
}

int Console::run(std::istream *in, std::ostream *out) {
    std::thread t1{&Console::runRx, this, out};
    int status = runTx(in);
    t1.join();
    return status;
}

void Console::compound(uint8_t cmd, std::vector<uint8_t> &data)
{
    Message m{data};
    m.insert(m.begin(), cmd);
    m.insert(m.begin(), 0x6);
    push(m);
}

void Console::compound(uint8_t cmd, uint8_t data)
{
    push(Message{0x6, cmd, data});
}

void Console::simple(uint8_t cmd)
{
    push(Message{0x6, cmd});
}

static unsigned getUint8(const uint8_t **ptr) 
{
    unsigned ret = **ptr;
    ++(*ptr);
    return ret;
}

static uint32_t getUint32(const uint8_t **ptr) 
{
    uint32_t ret = **ptr;
    ++(*ptr);
    ret |= (**ptr) << 8;
    ++(*ptr);
    ret |= (**ptr) << 16;
    ++(*ptr);
    ret |= (**ptr) << 24;
    ++(*ptr);
    return ret;
}

static uint16_t getUint16(const uint8_t **ptr) 
{
    uint16_t ret = **ptr;
    ++(*ptr);
    ret |= (**ptr) << 8;
    ++(*ptr);
    return ret;
}

static std::string getAddr(const uint8_t **ptr)
{
    std::stringstream s;
    s << "\"";
    for (int i=0; i < 8; ++i) {
        if (i) s << ':';
        s << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(**ptr);
        ++(*ptr);
    }
    s << "\"";
    return s.str();
}

void Console::decode(const Message &msg, std::ostream &out)
{
    switch (msg[0]) {
        case '\x20':
            out << "{ \"mode\":\"" 
                << (msg[1] == 1 ? "LBR" : "NLBR")
                << "\", \"neighbors\":" 
                << std::dec << static_cast<unsigned>(msg[2])
                << ", \"discoveryState\":"
                << std::dec << static_cast<unsigned>(msg[3])
                << " }\n";
                break;
        case '\x21':  // diag
            switch (msg[1]) {
                case 1:  // DIAG_ID_ADDRESS_INFO
                    if (msg.size() != 34) {
                        out << "Error: bad diag 1 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"addrinfo\": { ";
                        out << "\"time\":" << std::dec << getUint32(&ptr);
                        out << ", \"acceptedframes\":" << getUint32(&ptr);
                        out << ", \"rejectedaddresses\":" << getUint16(&ptr);
                        out << ", \"recvdmhr\":" << getUint16(&ptr);
                        out << ", \"lastrcvddstpanid\":" << getUint16(&ptr);
                        out << ", \"lastrcvdsrcpanid\":" << getUint16(&ptr);
                        out << ", \"lastrejectaddr\":" << getAddr(&ptr);
                        out << ", \"lastrcvdaddr\":" << getAddr(&ptr);
                        out << " } }\n";
                    }
                    break;
                case 2:  // DIAG_ID_IE_COUNTS
                    if (msg.size() != 58) {
                        out << "Error: bad diag 2 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"iecounters\": { ";
                        out << "\"fcie\":" << std::dec << getUint32(&ptr);
                        out << ", \"uttie\":" << getUint32(&ptr);
                        out << ", \"rslie\":" << getUint32(&ptr);
                        out << ", \"btie\":" << getUint32(&ptr);
                        out << ", \"usie\":" << getUint32(&ptr);
                        out << ", \"bsie\":" << getUint32(&ptr);
                        out << ", \"panie\":" << getUint32(&ptr);
                        out << ", \"netnameie\":" << getUint32(&ptr);
                        out << ", \"panverie\":" << getUint32(&ptr);
                        out << ", \"gtkhashie\":" << getUint32(&ptr);
                        out << ", \"mpie\":" << getUint32(&ptr);
                        out << ", \"mhdsie\":" << getUint32(&ptr);
                        out << ", \"vhie\":" << getUint32(&ptr);
                        out << ", \"vpie\":" << getUint32(&ptr);
                        out << " } }\n";
                    }
                    break;
                case 3:  // DIAG_ID_IE_UNKNOWN
                    if (msg.size() != 46) {
                        out << "Error: bad diag 2 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"ieunknown\": { ";
                        out << "\"count\":" << std::dec << getUint32(&ptr)
                            << ", \"rejected\": [ ";
                        for (int i=0; i<10; ++i) {
                            if (i) out << ", ";
                            out << "{ \"desc\":" << getUint16(&ptr);
                            out << ", \"subdesc\":" << getUint16(&ptr) << "}";
                        }
                        out << " ] } }\n";
                    }
                    break;
#if 0
                case 4:  // DIAG_ID_MPX_INFO
                    break;
                case 5:  // DIAG_ID_FH_IE_INFO
                    break;
                case 6:  // DIAG_ID_FH_MY_SEQ
                    break;
                case 7:  // DIAG_ID_FH_NB_SEQ
                    break;
                case 8:  // DIAG_ID_FH_NB_INFO
                    break;
#endif
                case 9:  // DIAG_ID_RADIO_STATS
                    if (msg.size() != 45) {
                        out << "Error: bad diag 9 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"radiostats\": { ";
                        out << "\"rxcount\":" << std::dec << getUint32(&ptr);
                        out << ", \"fifoerrors\":" << getUint32(&ptr);
                        out << ", \"crcerrors\":" << getUint32(&ptr);
                        out << ", \"rxinterrupts\":" << getUint32(&ptr);
                        out << ", \"lastrxlen\":" << getUint16(&ptr);
                        out << ", \"rssi\":" << getUint8(&ptr);
                        out << ", \"txinterrupts\":" << getUint32(&ptr);
                        out << ", \"spuriousints\":" << getUint32(&ptr);
                        out << ", \"txerrors\":" << getUint32(&ptr);
                        out << ", \"txpackets\":" << getUint32(&ptr);
                        out << ", \"txfifoerr\":" << getUint32(&ptr);
                        out << ", \"txchipstat\":" << getUint32(&ptr);
                        out << " } }\n";
                    }
                    break;
                default:  // DIAG_ID_INVALID
                    out << "Console received message: " << msg << '\n';
            }
            break;
        case '\x22':
            out << "{ \"buildid\":\"";
            std::copy(++msg.begin(), msg.end(), std::ostream_iterator<uint8_t>(out));
            out << "\" }\n";
            break;
        default:
            out << "Console received message: " << msg << '\n';
    }
}
