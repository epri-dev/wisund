#ifndef SLIP_H
#define SLIP_H
#include <cstdint>

void emit(uint8_t byte);
void send(const uint8_t *ptr, unsigned len);
#endif // SLIP_H
