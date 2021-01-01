
#ifndef _IMU_H
#define _IMU_H

#include <stdint.h>

#include "cpp/DS4Structs.h"

#ifdef __cplusplus
extern "C" {
#endif

struct imu_t;

struct imu_t* imu_init();
void          imu_free(struct imu_t*);
void          imu_set_calibration_data(struct imu_t* imu, const CalibrationData_t* data);
void          imu_update(struct imu_t* imu, const BTReport11_t* report);

#ifdef __cplusplus
}
#endif
  
#endif /* _IMU_H */
