#ifndef SLIP_H
#define SLIP_H
#include <stdint.h>

void emit(uint8_t byte);
void send(const uint8_t *ptr, unsigned len);
#endif // SLIP_H
