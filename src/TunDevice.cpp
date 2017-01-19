#include "TunDevice.h"
#include <thread>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/if_tun.h>

TunDevice::TunDevice(SafeQueue<Message> &input, SafeQueue<Message> &output) :
    Device(input, output),
    fd{0},
    m_verbose{false},
    m_ipv6only{true}
{
    struct ifreq ifr;
    int err;

    if ((fd = open("/dev/net/tun", O_RDWR)) == -1) {
        perror("open /dev/net/tun");
        exit(1);
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN;
    // don't let the kernel pick the name
    strncpy(ifr.ifr_name, "tun0", IFNAMSIZ);

    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) == -1) {
        perror("ioctl TUNSETIFF");
        close(fd);
        exit(1);
    }
    // After the ioctl call above, the fd is "connected" to tun device
    ioctl(fd, TUNSETNOCSUM, 1);
    // use system for these?
}

TunDevice::~TunDevice() {
    close(fd);
}

int TunDevice::runTx(std::istream *in)
{
    in = in;
    while (wantHold()) {
        startReceive(); 
    }
    return 0;
}

int TunDevice::runRx(std::ostream *out)
{
    out = out;
    Message m{};
    while (wantHold()) {
        wait_and_pop(m);
        send(m);
    }
    return 0;
}

int TunDevice::run(std::istream *in, std::ostream *out)
{
    in = in;
    out = out;
    std::thread t1{&TunDevice::runRx, this, out};
    int status = runTx(in);
    t1.join();
    return status;
}
/* 
 * sample packet
 *
 *  -4: 00 00 86 dd 
 * 00h: 60 0d f3 30 
 * 04h: 00 1f         // payload length
 * 06h: 3a 40 
 * 08h: 20 16 0b d8 
 *      00 00 f1 01 
 *      00 00 00 00 
 *      00 00 01 01 
 * 18h: 20 16 0b d8 
 *      00 00 f1 01
 *      00 00 00 00
 *      00 00 01 02 
 * 28h: 80 00 24 68 2e 83 00 11 77 c7 77 58 00 00 00 00 
 * 38h: ad 6d 08 00 00 00 00 00 10 11 12 13 14 15 16
 *
 */
bool TunDevice::isCompleteIpV6Msg(const Message& msg) const {
    if (m_verbose) {
        std::cout << "size = " << std::hex <<  msg.size() << ", ethertype = "
            << (0u + msg[2]) << (0u + msg[3]) << ", ipv6ver = "
            << (msg[4] & 0xf0u) << ", reported size = "
            << (msg[8] * 256u + msg[9] + 44) << "\n";
    }
    // if we're not being picky, accept everything
    if (!m_ipv6only) {
        return true;
    }
    return msg.size() >= 45 
        && (msg[2] == 0x86 && msg[3] == 0xdd) // IPv6 Ethertype
        && ((msg[4] & 0xf0) == 0x60)       // IPv6 version
        && (msg[8] * 256u + msg[9] + 44 == msg.size()) // complete
        ;
}

void TunDevice::startReceive()
{
    fd_set rfds;
    Message msg{};

    for (bool partialpkt{true}; partialpkt; partialpkt = msg.size() > 0) {
        // watch just our fd
        FD_ZERO(&rfds);
        FD_SET(fd, &rfds);

        // wait up to one half second
        timeval tv{0, 500000lu};
        auto retval = select(fd+1, &rfds, NULL, NULL, &tv);
        if (retval == -1) {
            // error in select
            std::cout << "TUN: error in select\n";
            return;
        }
        if (FD_ISSET(fd, &rfds)) { 
            uint8_t buf1[1600];
            size_t len = read(fd, buf1, sizeof(buf1));
            msg += Message{buf1, len};
            if (isCompleteIpV6Msg(msg)) {
                push(msg);
            } 
        } else {
            msg.clear();
            //std::cout << "TUN: select timeout\n";
        }
    }
}

size_t TunDevice::send(const Message &msg)
{
    if (msg.size()) {
        write(fd, msg.data(), msg.size());
    }
    return msg.size();
}

bool TunDevice::verbosity(bool verbose) 
{
    std::swap(verbose, m_verbose);
    return verbose;
}

bool TunDevice::strict(bool strict)
{
    std::swap(strict, m_ipv6only);
    return strict;
}
