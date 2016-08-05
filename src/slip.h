#ifndef SLIP_H
#define SLIP_H
#include <stdint.h>

ssize_t slipsend(int fd, const void *ptr, size_t len);
ssize_t slipread(int fd, void *ptr, size_t len);
#endif // SLIP_H
