#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include "slip.h"

// give SLIP characters names
enum { END = 0xc0, ESC = 0xdb, ESC_END = 0xdc, ESC_ESC = 0xdd };

void emit(int fd, uint8_t byte)
{
    dprintf(STDERR_FILENO, "%2.2x ", byte);
    write(fd, &byte, 1);
}

ssize_t slipsend(int fd, const void *p, size_t len) 
{
    const uint8_t *ptr = p;
    // wrap the payload inside 0xC0 ... 0xC0 
    emit(fd, END);
    ssize_t sent;
    for (sent = 0 ; len; --len, ++ptr) {
        switch (*ptr) {
        case END:
            emit(fd, ESC);
            emit(fd, ESC_END);
            break;
        case ESC:
            emit(fd, ESC);
            emit(fd, ESC_ESC);
            break;
        default:
            emit(fd, *ptr);
        }
        ++sent;
    }
    emit(fd, END);
    return sent;
}

ssize_t slipread(int fd, void *p, size_t len)
{
    static uint8_t buff[1600];
    uint8_t *ptr = p;
    ssize_t retlen = 0;
    // undo the SLIP work
    ssize_t l = read(fd, buff, 1600);
    uint8_t prev = '\0';
    for (uint8_t *in = buff; l; --l) {
        switch(*in) {
            case END:
                // do nothing
                break;
            case ESC_END:
                if (prev == ESC) {
                    *ptr++ = END;
                    ++retlen;
                }
                break;
            case ESC_ESC:
                if (prev == ESC) {
                    *ptr++ = ESC;
                    ++retlen;
                }
                break;
            default:
                *ptr++ = *in;
                ++retlen;
        }
        prev = *in++;
        // don't overflow the passed buffer
        if ((size_t)retlen == len) {
            return retlen;
        }
    }
    return retlen;
} 
