#include "pcapng.h"
#include <iostream>
#include <iomanip>
#include <chrono>

namespace pcapng {
std::istream &Block::read(std::istream &in) {
    return in.read(reinterpret_cast<char *>(&len), sizeof *this - sizeof this->BlockType);
}

std::ostream& operator<<(std::ostream &out, const Block &b) {
    return out << std::hex 
        << "\nBlockType = 0x" << b.BlockType
        << "\nlen = 0x" << b.len
        << '\n';
}

std::istream &SHB::read(std::istream &in) {
    return in.read(reinterpret_cast<char *>(&len), sizeof *this - sizeof this->BlockType);
}

std::ostream &SHB::write(std::ostream &out) {
    out.write(reinterpret_cast<char *>(this), sizeof *this);
    return out.write(reinterpret_cast<char *>(&len), sizeof len);
}

std::ostream& operator<<(std::ostream &out, const SHB &b) {
    return out << std::hex
        << "\nBlockType = 0x" << b.BlockType
        << "\nlen = 0x" << b.len
        << "\nByteOrderMagic = 0x" << b.ByteOrderMagic
        << "\nMajorVersion = 0x" << b.MajorVersion
        << "\nMinorVersion = 0x" << b.MinorVersion
        << "\nSectionLen = 0x" << b.SectionLen
        << '\n';
}

std::istream &IDB::read(std::istream &in) {
    return in.read(reinterpret_cast<char *>(&len), sizeof *this - sizeof this->BlockType);
}

std::ostream &IDB::write(std::ostream &out) {
    out.write(reinterpret_cast<char *>(this), sizeof *this);
    return out.write(reinterpret_cast<char *>(&len), sizeof len);
}

std::ostream& operator<<(std::ostream &out, const IDB &b) {
    return out << std::hex 
        << "\nBlockType = 0x" << b.BlockType
        << "\nlen = 0x" << b.len
        << "\nLinkType = 0x" << b.LinkType
        << "\nReserved = 0x" << b.Reserved
        << "\nSnapLen = 0x" << b.SnapLen
        << '\n';
}

std::istream &EPB::read(std::istream &in) {
    return in.read(reinterpret_cast<char *>(&len), sizeof *this - sizeof this->BlockType);
}

std::ostream &EPB::write(std::ostream &out, const uint8_t *pkt, std::size_t pktlen) {
    static constexpr uint32_t pad{0};
    std::size_t padsize{pktlen % sizeof pad ? sizeof pad - pktlen % sizeof pad : 0};
    std::cout << "pktlen = 0x" << std::hex << pktlen << "\npadsize = 0x" << padsize << "\n";
    CapturedLen = OriginalLen = pktlen;
    len += pktlen + padsize;
    out.write(reinterpret_cast<const char *>(this), sizeof *this);
    out.write(reinterpret_cast<const char *>(pkt), pktlen);
    out.write(reinterpret_cast<const char *>(&pad), padsize);
    return out.write(reinterpret_cast<const char *>(&len), sizeof len);
}

std::ostream& operator<<(std::ostream &out, const EPB &b) {
    return out << std::hex 
        << "\nBlockType = 0x" << b.BlockType
        << "\nlen = 0x" << b.len
        << "\nInterfaceID = 0x" << b.InterfaceID
        << "\nTimestampHi = 0x" << b.TimestampHi
        << "\nTimestampLo = 0x" << b.TimestampLo
        << "\nCapturedLen = 0x" << b.CapturedLen
        << "\nOriginalLen = 0x" << b.OriginalLen
        << '\n';
}

// TODO: fix timestamp to always work
/* Right now, the system_clock epoch is assumed to be
 * 1 Jan 1970 00:00:00 which is what the PCAPNG file 
 * requires, but that is implementation defined so we
 * really ought to make a check for that.
 */
void EPB::stamp() {
    uint64_t now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    TimestampHi = now >> 32;
    TimestampLo = now & 0xffffffffu;
}
}
