
#ifndef _DS5STRUCTS_H
#define _DS5STRUCTS_H

#ifdef TARGET_PC
#include <stdint.h>
#endif

#include "Utils.h"

#ifdef WIN32
#pragma pack(push, 1)
#endif

typedef struct
{
  uint8_t  reportId;                                 // 0x01

  uint8_t LPadX;                                     // 0x00 left 0xFF right
  uint8_t LPadY;                                     // 0x00 up   0xFF down
  uint8_t RPadX;
  uint8_t RPadY;
  uint8_t L2Value;
  uint8_t R2Value;
  uint8_t Counter1;

  uint8_t Hat : 4;
  uint8_t Square : 1;
  uint8_t Cross : 1;
  uint8_t Circle : 1;
  uint8_t Triangle : 1;

  uint8_t R3 : 1;
  uint8_t L3 : 1;
  uint8_t Options : 1;
  uint8_t Share : 1;
  uint8_t L1 : 1;
  uint8_t R1 : 1;
  uint8_t L2 : 1;
  uint8_t R2 : 1;

  uint8_t  BTN_GamePadButton13 : 1;                  // Usage 0x0009000D: Button 13, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton14 : 1;                  // Usage 0x0009000E: Button 14, Value = 0 to 1, Physical = Value x 315
  uint8_t  BTN_GamePadButton15 : 1;                  // Usage 0x0009000F: Button 15, Value = 0 to 1, Physical = Value x 315
  uint8_t  VEN_GamePad0021 : 1;                      // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t  VEN_GamePad00211 : 1;                     // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t  Mute : 1;
  uint8_t  TPad : 1;
  uint8_t  PS : 1;

  uint8_t VEN_GamePad00215 : 1;                     // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad00216 : 1;                     // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad00217 : 1;                     // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad00218 : 1;                     // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad00219 : 1;                     // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad002110 : 1;                    // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad002111 : 1;                    // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315
  uint8_t VEN_GamePad002112 : 1;                    // Usage 0xFF000021: , Value = 0 to 1, Physical = Value x 315

  uint32_t Counter2; // Not 100% about the highest byte. May be a 4ms timestamp instead of a counter.

  uint16_t gyroX;
  uint16_t gyroY;
  uint16_t gyroZ;
  uint16_t accelX;
  uint16_t accelY;
  uint16_t accelZ;
  // XXXFIXME actually the sensor timestamp is here as uint32_t (1/3 microseconds)
  uint8_t GyroOthers[2];

  uint16_t Timestamp; // Or is it ? Increments every 20ms

  uint8_t unknown01; // Probably battery, TODO; current 0x11/0x12 (unpaired), 0xfc/0xfd (paired)

  uint8_t  VEN_GamePad0022[31];                      // Usage 0xFF000022: , Value = 0 to 255, Physical = Value x 21 / 17
} USBReport01_t;

#ifdef __cplusplus
static_assert(sizeof(USBReport01_t) == 64, "Invalid USBReport01_t size");
#endif

#ifdef WIN32
#pragma pack(pop)
#endif

#endif /* _DS5STRUCTS_H */
