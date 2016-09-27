#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <asio.hpp>
#include <sstream>
#include "Message.h"

enum {
    DIAG_ID_ADDRESS_INFO=1,
    DIAG_ID_IE_COUNTS,
    DIAG_ID_IE_UNKNOWN,
    DIAG_ID_MPX_INFO,
    DIAG_ID_FH_IE_INFO,
    DIAG_ID_FH_MY_SEQ,
    DIAG_ID_FH_NB_SEQ,
    DIAG_ID_FH_NB_INFO,
    DIAG_RADIO_STATS
};

class AddrInfo {
public:
    std::string json(Message &msg) {
        std::stringstream ret;
        ret << "\"addrinfo\":{\"time\":" << updatedTime << ","
            "\"accepted\":"<< frameAcceptedCount << ","
            "\"rejected\":" << addrRejectCount << ","
            "\"mhr\":\"" << std::setfill('0') << std::hex << std::setw(4) << "\","
            "\"dstpanid\":\"f3d2\","
            "\"srcpanid\":\"f309\","
            "\"lastrecject\":\"0000000000000000\","
            "\"lastreceived\":\"f3231208dd32c42e\""
            "}";
        return ret.str();
    }
private:
    uint32_t updatedTime;
    uint32_t frameAcceptedCount;
    uint16_t addrRejectCount;
    uint16_t rcvdMHR;
    uint16_t lastRcvdDstPANID;
    uint16_t lastRcvdSrcPANID;
    uint8_t lastRejectAddr[8];
    uint8_t lastRecvdAddr[8];
};

#if 0
std::string diag(uint8_t cmd, uint8_t data) 
{
    std::string resp;
    Message{0x6, 0x21, data}.send();
    // only a few cases send back data
    switch(data) {
        // TODO: take explicit care of endian-ness conversions
        case DIAG_ID_ADDRESS_INFO:
            Message m{0x78, 0x56, 0x34, 0x12,   // time
                      0xef, 0xcd, 0xab, 0x89,   // accepted
                      0xad, 0xde,               // rejected
                      0xef, 0xbe,               // rcvdMHR
                      0x34, 0x12,               // last dst PANID
                      0x56, 0x34,               // last src PANID
                      0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01,
                      0xa8, 0xa7, 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1
            };
            resp = std::string{"this is an answer\n"}; 
            break;
        case DIAG_ID_IE_COUNTS:
            Message reply();
            reply.receive();
            break;
        case DIAG_ID_IE_UNKNOWN:
            Message reply();
            reply.receive();
            break;
        case DIAG_RADIO_STATS:
            Message reply();
            reply.receive();
            break;
        default:
            // no reply
            break;
    }
    return resp;
}
#endif

class Serial {
public:
    Serial(const char *port, unsigned baud) 
        : m_io(), 
          m_port(m_io, port)
    {
        m_port.set_option(asio::serial_port_base::baud_rate(baud));
    }
    ~Serial() {
        m_port.close();
    }
    size_t receive(void *data, size_t length) {
        return m_port.read_some(asio::buffer(data, length));
    }
    size_t send(void *data, size_t length) {
        return m_port.write_some(asio::buffer(data, length));
    }
    size_t send(Message &msg) {
        return m_port.write_some(asio::buffer(msg.data(), msg.size()));
    }
    Message receive() {
        asio::streambuf buff;
        asio::streambuf::mutable_buffers_type mutableBuffer = buff.prepare(1024);
        std::vector<uint8_t> msg;
        size_t size = 0;
        bool started = false;
        bool ended = false;
        while (!ended) {
            size = m_port.read_some(mutableBuffer);
            std::vector<uint8_t> temp2;
            uint8_t *p1 = asio::buffer_cast<uint8_t *>(mutableBuffer);
            for (  ; size; --size) {
                temp2.push_back(*p1++);
            }
            for (auto byte : temp2) {
                if (started) {
                    if (byte == 0xc0) {
                        msg.push_back(byte); 
                        started = true;
                    }
                } else {
                    if (byte == 0xc0) {
                        ended = true;
                    }
                    msg.push_back(byte); 
                }
            }
        }
        return Message{msg};
    }
private:
    uint8_t read_msg_[1024];
    asio::io_service m_io;
    asio::serial_port m_port;
};

class IECounters {
public:
    static std::string json(Message &msg) {
        std::stringstream ret;
        if (msg.size() != 58) {
            return ret.str();
        }
        ret << "\"iecounters\":[";
        for (size_t i=1; i < msg.size(); i+=4) {
            unsigned val = msg[i+3];
            val = (val << 8) & msg[i+2];
            val = (val << 8) & msg[i+1];
            val = (val << 8) & msg[i+0];
            if (i != 1) {
                ret << ",";
            }
            ret << val;
        }


        return ret.str();
    }
};

class Diagnostics {
public:
    friend std::ostream& operator<<(std::ostream& out, const Diagnostics &d) {
        auto it=d.iecounts.begin();
        out << "{ 'iecounters':[" << *it;
        for ( ; it != d.iecounts.end(); ++it) {
            out << "," << *it;
        }
        out << "], 'addrinfo':{'time':32032,"
            "'accepted':2802,"
            "'rejected':0,"
            "'mhr':'321a',"
            "'dstpanid':'f3d2',"
            "'srcpanid':'f309',"
            "'lastrecject':'0000000000000000',"
            "'lastreceived':'f3231208dd32c42e'"
            "}}\n";
        return out;

    }
private:
    std::array<unsigned, 14> iecounts;
};

int main(int argc, char *argv[]) 
{
    if (argc != 3) {
        std::cout << "Usage: toy namedpipe serialport\n";
        return 1;
    }
    Diagnostics diags;
    std::cout << "Opening port " << argv[2] << "\n";
    Serial serial{argv[2], 115200};
    Message msg = Message{0x6, 0x21, 0x2}.encode();
    Message msg2 = Message{0x6, 0x21, 0x1}.encode();
    std::cout << "Opening pipe " << argv[1] << "\n";
    std::fstream pipe(argv[1]);

    while (pipe) {
        serial.send(msg);
        Message rawreply{serial.receive()};
        Message reply = rawreply.decode();
        std::string json = IECounters::json(reply);

        serial.send(msg2);
        Message rawreply2{serial.receive()};
        Message reply2 = rawreply.decode();
        AddrInfo ai;
        std::string json2 = ai.json(reply2);

        pipe << "{" << json << "," << json2 << "}" << std::endl;
        sleep(1);
    }
}
