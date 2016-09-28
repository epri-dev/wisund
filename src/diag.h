#ifndef DIAG_H
#define DIAG_H
#include <cstdint>
#include <string>
#include "Serial.h"
std::string diag(uint8_t cmd, uint8_t data, Serial &serial);
#endif /// DIAG_H
