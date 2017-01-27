#include "Reply.h"
#include "Message.h"
#include <iomanip>
#include <iterator>
#include <sstream>

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

static std::string getPanId(const uint8_t **ptr)
{
    std::stringstream s;
    s << "\"";
    for (int i=0; i < 2; ++i) {
        s << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(**ptr);
        ++(*ptr);
    }
    s << "\"";
    return s.str();
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

static std::string getExludedMask(const uint8_t **ptr)
{
    std::stringstream s;
    s << "\"";
    for (int i=0; i < 17; ++i) {
        s << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(**ptr);
        ++(*ptr);
    }
    s << "\"";
    return s.str();
}

static std::string getModeStr(uint8_t mode)
{
    std::stringstream s;
    if (mode == 0) s << "IDLE";
    else if (mode == 1) s << "LBR";
    else if (mode == 2) s << "NLBR";
    else s << "UNK";
    return s.str();
}

void decode(const Message &msg, std::ostream &out)
{
    if (msg.size() == 0) return;
    switch (msg[0]) {
        case '\x20':
            out << "{ \"mode\":\"" 
                << (getModeStr(msg[1]) )
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
                        out << ", \"lastrcvddstpanid\":" << getPanId(&ptr);
                        out << ", \"lastrcvdsrcpanid\":" << getPanId(&ptr);
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
                        out << "Error: bad diag 3 packet: " << msg << "\n";
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
                case 4:  // DIAG_ID_MPX_INFO
                    if (msg.size() != 32) {
                        out << "Error: bad diag 4 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"mpxie\": { ";
                        out << "\"timestamp\":" << std::dec << getUint32(&ptr);
                        out << ", \"mpxie_count\":" << std::dec << getUint32(&ptr);
                        out << ", \"mpx_id\":" << getUint16(&ptr);
                        out << ", \"mpx_subid\":" << getUint16(&ptr);
                        out << ", \"msdulength\":" << std::dec << getUint16(&ptr);
                        out << ", \"srcaddr\":" << getAddr(&ptr);
                        out << ", \"destaddr\":" << getAddr(&ptr);
                        out << " } }\n";
                    }
                    break;
                case 5:  // DIAG_ID_FH_IE_INFO
                    if (msg.size() != 48) {
                        out << "Error: bad diag 5 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"fhieinfo\": { ";
                        out << "\"index\":" << std::dec << getUint8(&ptr);
                        out << ", \"timestamp\":" << std::dec << getUint32(&ptr);
                        out << ", \"clock_drift\":" << std::dec << getUint8(&ptr);
                        out << ", \"timestamp_accuracy\":" << std::dec << getUint8(&ptr);
                        out << ", \"unicast_dwell\":" << std::dec << getUint8(&ptr);
                        out << ", \"broadcast_dwell\":" << std::dec << getUint8(&ptr);
                        out << ", \"broadcast_interval\":" << std::dec << getUint32(&ptr);
                        out << ", \"broadcast_schedule_id\":" << std::dec << getUint16(&ptr);
                        out << ", \"channel_plan\":" << std::dec << getUint8(&ptr);
                        out << ", \"channel_function\":" << getUint8(&ptr);
                        out << ", \"reg_domain\":" << std::dec << getUint8(&ptr);
                        out << ", \"operating_class\":" << std::dec << getUint8(&ptr);
                        out << ", \"ch0\":" << std::dec << getUint32(&ptr);
                        out << ", \"channelspacing\":" << std::dec << getUint16(&ptr);
                        out << ", \"number_channels\":" << std::dec << getUint16(&ptr);
                        out << ", \"fixed_channel\":" << std::dec << getUint16(&ptr);
                        out << ", \"excludedchanmask\":" << getExludedMask(&ptr);
                        out << " } }\n";
                    }
                    break;
                case 6:  // DIAG_ID_FH_MY_SEQ    
                    if (msg.size() != 3u+4*msg[2]) {
                        out << "Error: bad diag 6 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        int count = getUint8(&ptr);
                        out << "{ \"mysequence\": [ " << std::dec;
                        for (int i=0; i < count; ++i) {
                            if (i) out << ", ";
                            out << "{ \"slot\":" << getUint16(&ptr);
                            out << ", \"chan\":" << getUint16(&ptr) << "}";
                        }
                        out << " ] }\n";
                    }
                    break;
                case 7:  // DIAG_ID_FH_NB_SEQ
                    if (msg.size() != 3u+4*msg[2]) {
                        out << "Error: bad diag 7 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        int count = getUint8(&ptr);
                        out << "{ \"targetseq\": [ " << std::dec;
                        for (int i=0; i < count; ++i) {
                            if (i) out << ", ";
                            out << "{ \"slot\":" << getUint16(&ptr);
                            out << ", \"chan\":" << getUint16(&ptr) << "}";
                        }
                        out << " ] }\n";
                    }
                    break;

                case 8:  // DIAG_ID_FH_NB_INFO
                    if (msg.size() != 59) {
                        out << "Error: bad diag 8 packet: " << msg << "\n";
                    } else {
                        const uint8_t *ptr = &msg[2];
                        out << "{ \"fhnbinfo\": { ";
                        out << "\"index\":" << std::dec << getUint8(&ptr);
                        out << ", \"timestamp\":" << std::dec << getUint32(&ptr);
                        out << ", \"lastrcvdufsi\":" << getUint32(&ptr);
                        out << ", \"neighborlastms\":"<< std::dec  << getUint32(&ptr);
                        out << ", \"lastrslrssi\":" << getUint8(&ptr);
                        out << ", \"rawmeasrssi\":" << getUint8(&ptr);
                        out << ", \"lastrcvdbfi\":" << getUint32(&ptr);
                        out << ", \"lastrcvdbsn\":" << std::dec << getUint16(&ptr);
                        out << ", \"panid\":" << getPanId(&ptr);
                        out << ", \"panversion\":" << std::dec << getUint16(&ptr);
                        // gtkhash is 4 x 64-bit unsigned.  DOn't know how to 
                        // do unsigned 64-bit int as uint64_t doesn't work?
                        out << " } }\n";
                    }
                    break;

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
        case  0xD0:
            out << "{ \"TEST_PRINTF\":\"";
            std::copy(++msg.begin(), msg.end(), std::ostream_iterator<uint8_t>(out));
            out << "\" }\n";
            break;
        case '\x23':
            if (msg.size() != 2u+14*msg[1]) {
                out << "Error: bad neighbors packet: " << msg << "\n";
            } else {
                const uint8_t *ptr = &msg[1];
                int count = getUint8(&ptr);
                out << "{ \"neighbors\": [ " << std::dec;
                for (int i=0; i < count; ++i) {
                    if (i) out << ", ";
                    out << "{ \"index\":" << getUint8(&ptr);
                    out << ", \"validated\":" << getUint8(&ptr);
                    out << ", \"timestamp\":" << getUint32(&ptr);
                    out << ", \"mac\":" << getAddr(&ptr) << "}";
                }
                out << " ] }\n";
            }
            break;
        case '\x24':
            if (msg.size() != 9) {
                out << "Error: mac packet: " << msg << "\n";
            } else {
                const uint8_t *ptr = &msg[1];
                out << "{ \"mac\":" << getAddr(&ptr) << " }\n";
            }
            break;
        default:
            out << "unknown reply: " << msg << '\n';
    }
}

std::string decode(const Message &msg) {
    std::stringstream ss;
    decode(msg, ss);
    return ss.str();
}
