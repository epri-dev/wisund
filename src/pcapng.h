#ifndef PCAPNG_H
#define PCAPNG_H
#include <iostream>

/*
 * The minimal PCAPNG file contains one block (SHB) and no data.  
 * A more functional one contains 1 SHB, 1 IDB and one or more EPBs.
 *
 * This is a very rudimentary implementation of a PCAPNG file writer.
 */
namespace pcapng {
// TODO: enable use of options in blocks
class Option {
    uint16_t OptionCode;
    uint16_t len;
} __attribute__((packed));

class Block {
public:
    uint32_t BlockType;
    uint32_t len;
    Block(uint32_t block, uint32_t len=0) : BlockType{block}, len{len} {}
    std::istream &read(std::istream &in);
    friend std::ostream& operator<<(std::ostream &out, const Block &b);
} __attribute__((packed));

// Section Header Block
class SHB : public Block {
public:
    uint32_t ByteOrderMagic;
    uint16_t MajorVersion;
    uint16_t MinorVersion;
    int64_t SectionLen;

    SHB() : 
        Block{0x0a0d0d0a, sizeof *this + sizeof len}, 
        ByteOrderMagic{0x1A2B3C4D},
        MajorVersion{1}, 
        MinorVersion{0}, 
        SectionLen{-1}
    {}
    std::istream &read(std::istream &in);
    std::ostream &write(std::ostream &out);
    friend std::ostream& operator<<(std::ostream &out, const SHB &b);

} __attribute__((packed));

// Interface Description Block
class IDB : public Block {
public:
    uint16_t LinkType;
    uint16_t Reserved;
    uint32_t SnapLen;

    IDB() : Block{1, sizeof *this + sizeof len},
        LinkType{230},  // 	IEEE 802.15.4 wireless Personal Area Network, without the FCS at the end of the frame.   See http://www.tcpdump.org/linktypes.html
        Reserved{0},
        SnapLen{0xffff}
    {}
    std::istream &read(std::istream &in);
    std::ostream &write(std::ostream &out);
    friend std::ostream& operator<<(std::ostream &out, const IDB &b);
} __attribute__((packed));

// Enhanced Packet Block
class EPB : public Block {
public:
    uint32_t InterfaceID;
    uint32_t TimestampHi;
    uint32_t TimestampLo;
    uint32_t CapturedLen;
    uint32_t OriginalLen;

    EPB() : Block{6, sizeof *this + sizeof len},
        InterfaceID{0},
        TimestampHi{0}, 
        TimestampLo{0},
        CapturedLen{0},
        OriginalLen{0}
    { 
        stamp(); 
    }
    std::istream &read(std::istream &in);
    std::ostream &write(std::ostream &out, const uint8_t *pkt, std::size_t pktlen);
    void stamp();
    friend std::ostream& operator<<(std::ostream &out, const EPB &b);
} __attribute__((packed));

}

#endif // PCAPNG_H
