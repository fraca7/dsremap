
// Include this file first everywhere

#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include "messages.h"

// Serial for logs
#define LOG_SERIAL Serial1

// Baudrate for the serial interface
#define LOG_SERIAL_BAUDRATE 74880

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERR  1
#define LOG_LEVEL_WARN 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4
#define LOG_LEVEL_TRACE 5

#define LOG_LEVEL LOG_LEVEL_NONE

// Sometimes you want to make room for logs. This disables the whole
// input report remapping (along with the VM and EEPROM-associated
// code).
// #define DISABLE_REMAPPING

#include "Log.h"

// Targeted platform
#define TARGET_PS4 0
#define TARGET_PS5 1

#define TARGET_PLATFORM TARGET_PS4

#endif /* _CONFIG_H */
