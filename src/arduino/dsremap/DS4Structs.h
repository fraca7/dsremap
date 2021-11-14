
#ifndef _DS4STRUCTS_H
#define _DS4STRUCTS_H

#ifdef TARGET_PC
#include <stdint.h>
#endif

#include "Utils.h"

#define DS4_COMMON_FIELDS                                       \
  uint8_t LPadX; /* 0x00: left; 0xFF: right */                  \
  uint8_t LPadY; /* 0x00: up; 0xFF: down */                     \
  uint8_t RPadX;                                                \
  uint8_t RPadY;                                                \
                                                                \
  uint8_t Hat : 4; /* 0x08 released, 0x00 N, 0x01 NE, etc */    \
  uint8_t Square : 1;                                           \
  uint8_t Cross : 1;                                            \
  uint8_t Circle : 1;                                           \
  uint8_t Triangle : 1;                                         \
                                                                \
  uint8_t L1 : 1;                                               \
  uint8_t R1 : 1;                                               \
  uint8_t L2 : 1;                                               \
  uint8_t R2 : 1;                                               \
  uint8_t Share : 1;                                            \
  uint8_t Options : 1;                                          \
  uint8_t L3 : 1;                                               \
  uint8_t R3 : 1;                                               \
                                                                \
  uint8_t PS : 1;                                               \
  uint8_t TPad : 1;                                             \
  uint8_t Counter : 6;                                          \
                                                                \
  uint8_t L2Value;                                              \
  uint8_t R2Value

#define DS4_MAIN_FIELDS                                 \
  uint16_t timestamp; /* 5.33 microseconds units */     \
  uint8_t unk01;                                        \
  int16_t gyroX;                                        \
  int16_t gyroY;                                        \
  int16_t gyroZ;                                        \
  int16_t accelX;                                       \
  int16_t accelY;                                       \
  int16_t accelZ;                                       \
  uint8_t unk02[5];                                     \
                                                        \
  /* When running on battery, range is 0..9.            \
     When connected to power, 0..10. More than 10 and   \
     connected means charge completed. */               \
  uint8_t batteryLevel : 4;                             \
  uint8_t cableState : 1;                               \
  uint8_t unk03 : 3;                                    \
                                                        \
  uint8_t unk04[2];                                     \
                                                        \
  uint8_t touchEventCount; /* Max 3 in USB, 4 in BT */  \
  uint8_t unk05                                         \

#ifdef WIN32
#pragma pack(push, 1)
#endif

typedef struct PACKED {
  uint8_t id; // 0x01

  DS4_COMMON_FIELDS;
} BTReport01_t;

typedef struct PACKED {
  uint8_t counter : 7;
  uint8_t released : 1; // 0: pressed; 1: not pressed
  uint8_t data[3];

#ifdef __cpluscplus
  uint16_t x() const {
    return data[0] | ((data[1] & 0x0F) << 8);
  }

  uint16_t y() const {
    return ((data[1] & 0xF0) >> 4) | (data[2] << 4);
  }
#endif
} TPadPosition;

typedef struct PACKED {
  uint8_t timestamp;
  TPadPosition prev;
  TPadPosition next;
} TPadEvent;

typedef struct PACKED {
  DS4_COMMON_FIELDS;
  DS4_MAIN_FIELDS;
  TPadEvent touchEvents[3];
} MainReport_t;

typedef struct PACKED {
  uint8_t id; // 0x11
  uint8_t btid01; // 0xC0
  uint8_t btid02; // 0x00

  DS4_COMMON_FIELDS;
  DS4_MAIN_FIELDS;
  TPadEvent touchEvents[4];

  uint8_t unk06[5];
} BTReport11_t;

#ifdef __cplusplus
static_assert(sizeof(BTReport11_t) == 78, "Invalid BTReport11_t size");
#endif

typedef struct PACKED {
  uint8_t id; // 0x01

  DS4_COMMON_FIELDS;
  DS4_MAIN_FIELDS;
  TPadEvent touchEvents[3];

  uint8_t unk06[2];
} USBReport01_t;

#ifdef __cplusplus
static_assert(sizeof(USBReport01_t) == 64, "Invalid USBReport01_t size");
#endif

#ifdef WIN32
#pragma pack(pop)
#endif

#define CONTROLLER_STATE_FROM_BUFFER(state, data) memcpy((uint8_t*)(state), (uint8_t*)(data) + 1, sizeof(*state))
#define CONTROLLER_STATE_TO_BUFFER(state, data) memcpy((uint8_t*)(data) + 1, (uint8_t*)(state), sizeof(*state))
#define IMU_STATE_FROM_BUFFER(imu, data) ((imu_state_t*)((uint8_t*)(data) + 10))

#endif /* _DS4STRUCTS_H */
