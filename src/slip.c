#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <unistd.h>
#include "slip.h"

// give SLIP characters names
enum { END = 0xc0, ESC = 0xdb, ESC_END = 0xdc, ESC_ESC = 0xdd };

void emit(uint8_t byte)
{
    dprintf(STDERR_FILENO, "%2.2x ", byte);
    write(STDOUT_FILENO, &byte, 1);
}

void send(const uint8_t *ptr, unsigned len) {
    // wrap the payload inside 0xC0 ... 0xC0 
    emit(END);
    for ( ; len; --len, ++ptr) {
        switch (*ptr) {
        case END:
            emit(ESC);
            emit(ESC_END);
            break;
        case ESC:
            emit(ESC);
            emit(ESC_ESC);
            break;
        default:
            emit(*ptr);
        }
    }
    emit(END);
}
