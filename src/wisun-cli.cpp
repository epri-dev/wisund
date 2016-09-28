#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <sstream>
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
        std::cout << "Usage: wisun-cli serialport\n";
        return 1;
    }
    std::cout << "Opening port " << argv[1] << "\n";
    Serial serial{argv[1], 115200};
    Message msg = Message{0x6, 0x21, 0x2}.encode();
    Message msg2 = Message{0x6, 0x21, 0x1}.encode();

    while (std::cin) {
        serial.send(msg);
        Message rawreply{serial.receive()};
        Message reply = rawreply.decode();
        std::string json = IECounters::json(reply);

        serial.send(msg2);
        Message rawreply2{serial.receive()};
        Message reply2 = rawreply.decode();
        AddrInfo ai;
        std::string json2 = ai.json(reply2);

        std::cout << "{" << json << "," << json2 << "}" << std::endl;
        sleep(1);
    }
}
