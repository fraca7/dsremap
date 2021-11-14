
#ifndef _COMMONSTRUCTS_H_
#define _COMMONSTRUCTS_H_

#ifdef TARGET_PC
#include <stdint.h>
#endif

#include "Utils.h"

#ifdef __cplusplus
enum class ButtonState {
  Released,
  Pressed
};
#endif

typedef struct {
  float x;
  float y;
  float z;
} Vector3D;

/*
 * The structures the VM operates on. Getting this from a Dualshock
 * report only involves pointer arithmetic; some values has to be
 * swapped around in the Dualsense case.
 */

#ifdef WIN32
#pragma pack(push, 1)
#endif

typedef struct PACKED {
  uint8_t LPadX;
  uint8_t LPadY;
  uint8_t RPadX;
  uint8_t RPadY;

  uint8_t Hat : 4;
  uint8_t Square : 1;
  uint8_t Cross : 1;
  uint8_t Circle : 1;
  uint8_t Triangle : 1;

  uint8_t L1 : 1;
  uint8_t R1 : 1;
  uint8_t L2 : 1;
  uint8_t R2 : 1;
  uint8_t Share : 1;
  uint8_t Options : 1;
  uint8_t L3 : 1;
  uint8_t R3 : 1;

  uint8_t PS : 1;
  uint8_t TPad : 1;
  uint8_t unused : 6;

  uint8_t L2Value;
  uint8_t R2Value;
} controller_state_t;

typedef struct PACKED {
  uint16_t timestamp;
  uint8_t unused;
  int16_t gyroX;
  int16_t gyroY;
  int16_t gyroZ;
  int16_t accelX;
  int16_t accelY;
  int16_t accelZ;
} imu_state_t;

typedef struct PACKED {
  int16_t gyro_x_bias;
  int16_t gyro_y_bias;
  int16_t gyro_z_bias;
  int16_t gyro_x_plus;
  int16_t gyro_x_minus;
  int16_t gyro_y_plus;
  int16_t gyro_y_minus;
  int16_t gyro_z_plus;
  int16_t gyro_z_minus;
  int16_t gyro_speed_plus;
  int16_t gyro_speed_minus;
  int16_t acc_x_plus;
  int16_t acc_x_minus;
  int16_t acc_y_plus;
  int16_t acc_y_minus;
  int16_t acc_z_plus;
  int16_t acc_z_minus;
  int16_t unk01;
} CalibrationData_t;

#ifdef __cplusplus
static_assert(sizeof(CalibrationData_t) == 36, "Invalid CalibrationData_t size");
#endif

#ifdef WIN32
#pragma pack(pop)
#endif

#endif
