#include "diag.h"
#include "Message.h"
#include "Serial.h"

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

class IECounters {
public:
    static std::string json(Message &msg) {
        std::stringstream ret;
        if (msg.size() != 58) {
            return ret.str();
        }
        ret << "\"iecounters\":[";
        for (size_t i=2; i < msg.size(); i+=4) {
            unsigned val = msg[i+3];
            val = (val << 8) | msg[i+2];
            val = (val << 8) | msg[i+1];
            val = (val << 8) | msg[i+0];
            if (i != 2) {
                ret << ",";
            }
            ret << val;
        }
        ret << "]";
        return ret.str();
    }
};

std::string diag(uint8_t cmd, uint8_t data, Serial &serial) {
    if (cmd != 0x21) {
        return "Error - bad cmd\n";
    }
    std::cout << "diag " << (unsigned)data << "\n";
    std::string resp;
    Message m = Message{0x6, cmd, data};
    // only a few cases send back data
    switch(data) {
        case DIAG_ID_IE_COUNTS:
            {
                serial.send(m);
                Message reply = serial.receive();
                std::cout << "reply = " << reply << "\n";
                std::cout << "message len = " << std::dec << reply.size() << "\n";
                resp = IECounters::json(reply);
            }
            break;
        default:
            // no reply
            break;
    }
    return resp;
}
