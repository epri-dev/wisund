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
    Device(input, output)
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
    Message m{0};
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

void TunDevice::startReceive()
{
    fd_set rfds;
    // watch just our fd
    FD_ZERO(&rfds);
    FD_SET(fd, &rfds);

    // wait up to one half second
    timeval tv{0, 500000lu};
    auto retval = select(fd+1, &rfds, NULL, NULL, &tv);
    if (retval == -1) {
        std::cout << "TUN: error in select\n";
        // error in select
    }
    if (FD_ISSET(fd, &rfds)) { 
        uint8_t buf1[1600];
        size_t len = read(fd, buf1, sizeof(buf1));
        push(Message{buf1, len});
    } else {
        //std::cout << "TUN: select timeout\n";
    }
}

size_t TunDevice::send(const Message &msg)
{
    if (msg.size()) {
        write(fd, msg.data(), msg.size());
    }
    return msg.size();
}
