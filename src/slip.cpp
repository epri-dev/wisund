#include "slip.h"
#include <iostream>
#include <iomanip>

void emit(uint8_t byte)
{
    std::cerr << std::setw(2) << std::setfill('0') << std::hex << byte << ' ';
    putchar(byte);
}


void send(const uint8_t *ptr, unsigned len) {
    // wrap the payload inside 0xC0 ... 0xC0 
    emit('\xC0');
    for (  ; len; ++ptr, --len) {
        switch(*ptr) {
            case 0xC0:
                emit(0xDB);
                emit(0xDC);
                break;
            case 0xDB:
                emit(0xDB);
                emit(0xDD);
                break;
            default:
                emit(*ptr);
        }
    }
    emit('\xC0');
}
