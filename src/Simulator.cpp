#include "Simulator.h"
#include <thread>
#include <map>
#include <stdexcept>

/// constructor takes references to input and output queues, serial port and baud rate
Simulator::Simulator(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    m_verbose{false},
    m_delay{0},
    lastMessage{}
{}

/// destructor is virtual in case class needs to be further derived
Simulator::~Simulator() = default;

/// runs both the receive and transmit handlers in required sequence
int Simulator::run(std::istream *in, std::ostream *out)
{
    in = in;
    out = out;
    Message m{};
    while (wantHold()) {
        wait_and_pop(m);
        send(m);
        receive();
    }
    return 0;
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

/// container for messages that get replies
struct cmdDetails {
    Message response;
    const char *command;
    const char *json;
};

static std::map<Message, cmdDetails> simCmd =
{
    {
        {{0x06,0x22}}, { {{ 0x22,0x4a,0x61,0x6e,0x20,0x33,0x30,0x20,0x32,0x30,0x31,0x37,0x20,0x30,0x39,0x3a,0x31,0x35,0x3a,0x35,0x38,0x20,0x31,0x76,0x30,0x31,0x44,0x30,0x34}}, 
        "buildid", R"({ "buildid":"Jan 30 2017 09:15:58 1v01D04" })", }
    },
    { 
        {{0x06, 0x20}}, { {{0x20, 0x01, 0x01, 0x04}},
        "state", R"({ "mode":"LBR", "neighbors":1, "discoveryState":4 })", }
    },  
    { 
        {{0x06,0x24}}, { {{0x24,0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x01}},
        "mac", R"({ "mac":"00:19:59:ff:fe:0f:ff:01" })", }
    },
    { 
        {{0x06, 0x21, 0x01}}, { {{0x21,0x01,0x42,0xfc,0x42,0x00,0x6f,0x00,0x00,0x00,0x00,0x00,0x01,0xe3,0x00,0x00,0xfe,0xca,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x02}},
        "diag 01", R"({ "addrinfo": { "time":4389954, "acceptedframes":111, "rejectedaddresses":0, "recvdmhr":58113, "lastrcvddstpanid":"0000", "lastrcvdsrcpanid":"feca", "lastrejectaddr":"00:00:00:00:00:00:00:00", "lastrcvdaddr":"00:19:59:ff:fe:0f:ff:02" } })", }
    },
    { 
        {{0x06, 0x21, 0x02}}, { {{0x21,0x02,0x00,0x00,0x00,0x00,0x73,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x73,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x6f,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}},
        "diag 02", R"({ "iecounters": { "fcie":0, "uttie":115, "rslie":0, "btie":4, "usie":115, "bsie":4, "panie":4, "netnameie":111, "panverie":4, "gtkhashie":4, "mpie":0, "mhdsie":0, "vhie":0, "vpie":0 } })", }
    },
    { 
        {{0x06, 0x21, 0x03}}, { {{0x21,0x03,0x37,0x00,0x00,0x00,0x74,0xc1,0x20,0x07,0x74,0xe1,0x20,0x07,0x74,0xe1,0x0e,0x05,0x74,0xd1,0x20,0x07,0xb4,0xe1,0x20,0x07,0xb4,0x91,0x0e,0x05,0x74,0xd1,0x0e,0x05,0x74,0xe1,0x0e,0x05,0x34,0x91,0x0e,0x05,0x74,0x91,0x0e,0x05,}},
        "diag 03", R"({ "ieunknown": { "count":55, "rejected": [ { "desc":49524, "subdesc":1824}, { "desc":57716, "subdesc":1824}, { "desc":57716, "subdesc":1294}, { "desc":53620, "subdesc":1824}, { "desc":57780, "subdesc":1824}, { "desc":37300, "subdesc":1294}, { "desc":53620, "subdesc":1294}, { "desc":57716, "subdesc":1294}, { "desc":37172, "subdesc":1294}, { "desc":37236, "subdesc":1294} ] } })", }
    },
    { 
        {{0x06, 0x21, 0x04}}, { {{0x21,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}},
        "diag 04", R"({ "mpxie": { "timestamp":0, "mpxie_count":0, "mpx_id":0, "mpx_subid":0, "msdulength":0, "srcaddr":"00:00:00:00:00:00:00:00", "destaddr":"00:00:00:00:00:00:00:00" } })", }
    },
    { 
        {{0x06, 0x21, 0x05}}, { {{0x21,0x05,0x00,0x13,0x56,0x4a,0x00,0x05,0x0a,0xfa,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x01,0x02,0x00,0xc5,0x0d,0x00,0x90,0x01,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}},
        "diag 05", R"({ "fhieinfo": { "index":0, "timestamp":4871699, "clock_drift":5, "timestamp_accuracy":10, "unicast_dwell":250, "broadcast_dwell":0, "broadcast_interval":0, "broadcast_schedule_id":0, "channel_plan":0, "channel_function":2, "reg_domain":1, "operating_class":2, "ch0":902400, "channelspacing":400, "number_channels":64, "fixed_channel":0, "excludedchanmask":"0000000000000000000000000000000000" } })", }
    },
    { 
        {{0x06, 0x21, 0x06}}, { {{0x21,0x06,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}},
        "diag 06", R"({ "mysequence": [ { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0} ] })", }
    },
    { 
        {{0x06, 0x21, 0x07}}, { {{0x21,0x07,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,}},
        "diag 07", R"({ "targetseq": [ { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0} ] })", }
    },
    { 
        {{0x06, 0x21, 0x08}}, { {{0x21,0x08,0x00,0x06,0x17,0x4d,0x00,0x1e,0x4a,0x43,0x00,0x61,0xb6,0x41,0x00,0x00,0x2b,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xca,0x01,0x00,0x21,0x43,0x65,0x87,0x00,0x00,0x00,0x00,0x22,0x43,0x65,0x87,0x00,0x00,0x00,0x00,0x24,0x43,0x65,0x87,0x00,0x00,0x00,0x00,0x24,0x43,0x65,0x87,0x00,0x00,0x00,0x00,}},
        "diag 08", R"({ "fhnbinfo": { "index":0, "timestamp":5052166, "lastrcvdufsi":4409886, "neighborlastms":4306529, "lastrslrssi":0, "rawmeasrssi":43, "lastrcvdbfi":0, "lastrcvdbsn":0, "panid":"feca", "panversion":1 } })", }
    },
    { 
        {{0x06, 0x21, 0x09}}, { {{0x21,0x09,0x7a,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7a,0x00,0x00,0x00,0x3a,0x00,0x2b,0x60,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x00,0x00,0x00,}},
        "diag 09", R"({ "radiostats": { "rxcount":122, "fifoerrors":0, "crcerrors":0, "rxinterrupts":122, "lastrxlen":58, "rssi":43, "txinterrupts":352, "spuriousints":0, "txerrors":0, "txpackets":352, "txfifoerr":0, "txchipstat":15 } })", }
    },
    { 
#if 0
        {{0x06,0x23,}}, { {{0x23,0x01,0x00,0x01,0x7e,0x02,0x4e,0x00,0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x02}},
        "neighbors", R"({ "neighbors": [ { "index":0, "validated":1, "timestamp":5112446, "mac":"00:19:59:ff:fe:0f:ff:02"} ] })", }
#else
        {{0x06,0x23,}}, { {{0x23,0x02,0x00,0x01,0x7e,0x02,0x4e,0x00,0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x02,0x01,0x01,0x7f,0x02,0x4e,0x00,0x00,0x19,0x59,0xff,0xfe,0x0f,0xff,0x03}},
        "neighbors", R"({ "neighbors": [ { "index":0, "validated":1, "timestamp":5112446, "mac":"00:19:59:ff:fe:0f:ff:02"}, { "index":1, "validated":1, "timestamp":5112447, "mac":"00:19:59:ff:fe:0f:ff:03"}  ] })", }
#endif
    },
};

/// receive simulated message from simulated device
void Simulator::receive() 
{
    static uint32_t fcie = 600;
    try {
        Message msg{simCmd.at(lastMessage).response};
        if (msg[0] == 0x21 && msg[1] == 2) {
            ++fcie;
            msg[2] = (fcie >> 0) & 0xff;
            msg[3] = (fcie >> 8) & 0xff;
            msg[4] = (fcie >> 16) & 0xff;
            msg[5] = (fcie >> 24) & 0xff;
        }
        if (m_verbose) {
            std::cout << "received: " << msg << "\n";
        }
        push(msg);
    }
    catch (std::out_of_range const &err) {
        // do nothing; it just means this command gets no response
    }
    // probably pedantic, but clear the last message
    lastMessage.clear();
}
