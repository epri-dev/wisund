#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include "slip.h"
#include "serialtun.h"

#define max(a,b) ((a)>(b) ? (a):(b))

int tun_open(char *devname)
{
    struct ifreq ifr;
    int fd, err;

    if ((fd = open("/dev/net/tun", O_RDWR)) == -1) {
        perror("open /dev/net/tun");
        exit(1);
    }
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN;
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);

    /* ioctl will use if_name as the name of TUN 
     * interface to open: "tun0", etc. */
    if ((err = ioctl(fd, TUNSETIFF, (void *)&ifr)) == -1) {
        perror("ioctl TUNSETIFF");
        close(fd);
        exit(1);
    }

    /* After the ioctl call the fd is "connected" to tun device specified
     * by devname */
    return fd;
}

int serial_open(char *ttyname)
{
    int fd = open(ttyname, O_RDWR);
    if (fd == -1) {
        perror("open serial port");
        exit(1);
    }
    // set essential serial parameters
    fcntl(fd, F_SETFL, 0);
    // std::cerr << "Port was successfully opened as "
    //    << std::dec << speed << ',' << data_bit << ',' << parity
    //    << ',' << (unsigned)stop_bit << std::endl;
    //tcgetattr(fd, &oldtio); // save current port settings
    struct termios newtio;
    memset(&newtio, 0, sizeof(newtio));
    newtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    newtio.c_iflag |= IGNPAR;
    newtio.c_iflag &= ~(IXON | IXOFF | IXANY);  // no flow control
    newtio.c_oflag &= ~OPOST;   // raw output mode
    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    newtio.c_cc[VMIN] = 0;      // require at least this many chars
    newtio.c_cc[VTIME] = 0;     // interchar timeout
    tcsetattr(fd, TCSANOW, &newtio);
    return fd;
}

void transcomm(bool do_slip)
{
    const int buflen = 1600;
    const int mtu = 1500;
    char buf1[buflen];
    char buf2[buflen];
    char *curr = buf2;
    char *end = &buf2[buflen];
    int f1, f2, fin, l, fm;
    fd_set fds;

    f1 = tun_open("tun0");
    f2 = serial_open("/dev/ttyAMA0");
    fin = STDIN_FILENO;

    // note that f1 and f2 are both
    // guaranteed to be greater than STDIN_FILENO
    fm = max(f1, f2) + 1;

    ioctl(f1, TUNSETNOCSUM, 1);

    while (1) {
        FD_ZERO(&fds);
        FD_SET(fin, &fds);
        FD_SET(f1, &fds);
        FD_SET(f2, &fds);

        select(fm, &fds, NULL, NULL, NULL);

        if (FD_ISSET(f1, &fds)) {
            l = read(f1, buf1, sizeof(buf1));
            if (do_slip) { 
                slipsend(f2, buf1, l);
            } else {
                write(f2, buf1, l);
            }
        }
        if (FD_ISSET(f2, &fds)) {
            if (do_slip) {
                l = slipread(f2, curr, end - curr);
            } else {
                l = read(f2, curr, end - curr);
            }
            curr += l;
            int ethertype = ntohs(*(uint16_t *) (&buf2[2]));
            int len = curr - buf2;
            int iplen = ntohs(*(uint16_t *) (&buf2[8]));
            if (ethertype == 0x86dd && (len - iplen) >= 44) {
                // only write a whole packet, since serial is much slower 
                write(f1, buf2, len > mtu ? mtu : len);
                memset(buf2, 0, 16);
                curr = buf2;
                if (len > mtu) {
                    len -= mtu;
                    memcpy(buf2, buf2 + mtu, len);
                    curr += len;
                }
            }
        }
        if (FD_ISSET(fin, &fds)) {
            close(f1);
            close(f2);
            return;
        }
    }
    close(f1);
    close(f2);
    return;
}
