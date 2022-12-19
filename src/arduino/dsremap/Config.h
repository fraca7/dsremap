
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

// The Last of Us part I (and thus probably part II) has some fucked
// up behavior regarding gyro aiming. Inverting the aiming axes also
// inverts the gyro (WTF). There is a separate configuration to
// "revert back" the gyro but it only works in Y.
// This works on PS5 because the DualSense input report signature does
// not include the gyro readings for some reason.

// This is NOT compatible with remapping

#ifdef DISABLE_REMAPPING
#define INVERT_GYRO_X
#define INVERT_GYRO_Y
//#define INVERT_GYRO_Z
#endif

#include "Log.h"

// Targeted platform
#define TARGET_PS4 0
#define TARGET_PS5 1

#define TARGET_PLATFORM TARGET_PS4

#endif /* _CONFIG_H */
