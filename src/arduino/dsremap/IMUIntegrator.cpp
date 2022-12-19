
#include "IMUIntegrator.h"

#ifdef TARGET_PC
#include <stdlib.h>
#endif

#define GYRO_RES 1024
#define ACCEL_RES 8192

/*
 * Multiplies an integer by a fraction, while avoiding unnecessary
 * overflow or loss of precision.
 * From linux/include/kernel.h
 */
#define mult_frac(x, numer, denom)(			\
{							\
	typeof(x) quot = (x) / (denom);			\
	typeof(x) rem  = (x) % (denom);			\
	(quot * (numer)) + ((rem * (numer)) / (denom));	\
}							\
)

#define EPSILON 0.01

IMUIntegrator::IMUIntegrator()
  : m_FirstTimestamp(true)
{
  m_Angles.x = m_Angles.y = m_Angles.z = 0.0f;
}

void IMUIntegrator::SetCalibrationData(const CalibrationData_t *pData)
{
  int32_t sp2x = (int32_t)pData->gyro_speed_plus + pData->gyro_speed_minus;

  m_Calib[0].bias = pData->gyro_x_bias;
  m_Calib[0].D = 2 * ((int32_t)pData->gyro_x_plus - pData->gyro_x_minus);
  m_Calib[1].bias = pData->gyro_y_bias;
  m_Calib[1].D = 2 * ((int32_t)pData->gyro_y_plus - pData->gyro_y_minus);
  m_Calib[2].bias = pData->gyro_z_bias;
  m_Calib[2].D = 2 * ((int32_t)pData->gyro_z_plus - pData->gyro_z_minus);

  m_Calib[0].N = sp2x * GYRO_RES;
  m_Calib[1].N = sp2x * GYRO_RES;
  m_Calib[2].N = sp2x * GYRO_RES;

  /*
  int32_t twoG;

  twoG = (int32_t)pData->acc_x_plus - pData->acc_x_minus;
  m_Calib[3].bias = (int32_t)pData->acc_x_plus - twoG / 2;
  m_Calib[3].D = twoG;

  twoG = (int32_t)pData->acc_y_plus - pData->acc_y_minus;
  m_Calib[4].bias = (int32_t)pData->acc_y_plus - twoG / 2;
  m_Calib[4].D = twoG;

  twoG = (int32_t)pData->acc_z_plus - pData->acc_z_minus;
  m_Calib[5].bias = (int32_t)pData->acc_z_plus - twoG / 2;
  m_Calib[5].D = twoG;

  m_Calib[3].N = 2 * ACCEL_RES;
  m_Calib[4].N = 2 * ACCEL_RES;
  m_Calib[5].N = 2 * ACCEL_RES;
  */
}

void IMUIntegrator::Update(imu_state_t* rep)
{
#ifndef DISABLE_REMAPPING
  // Compute timestamp in microseconds
  uint32_t delta = 0;

  if (m_FirstTimestamp) {
    m_FirstTimestamp = false;
  } else {
    if (m_LastTimestamp > rep->timestamp) {
      delta = (uint32_t)0xFFFF - m_LastTimestamp + rep->timestamp + 1;
    } else {
      delta = rep->timestamp - m_LastTimestamp;
    }
#if TARGET_PLATFORM == TARGET_PS5
    delta = delta * 4000;
#elif TARGET_PLATFORM == TARGET_PS4
    delta = delta * 16 / 3;
#endif

    if (delta == 0)
      // Happens from time to time on Bluetooth...
      return;
  }

  // NB: the first version used both the accelerometer & gyro
  // data. Turns out just using the gyro data is *way* more precise.

  float gx = (float)mult_frac(m_Calib[0].N, rep->gyroX - m_Calib[0].bias, m_Calib[0].D) / GYRO_RES;
  float gy = (float)mult_frac(m_Calib[1].N, rep->gyroY - m_Calib[1].bias, m_Calib[1].D) / GYRO_RES;
  float gz = (float)mult_frac(m_Calib[2].N, rep->gyroZ - m_Calib[2].bias, m_Calib[2].D) / GYRO_RES;

  if (abs(gx) < 1)
    gx = 0.0f;
  if (abs(gy) < 1)
    gy = 0.0f;
  if (abs(gz) < 1)
    gz = 0.0f;

  if (delta == 0) {
    // Ideally we'd get the readings from the accelerometer, but we
    // don't really care since we only want the angle delta relatively
    // to a given moment...
    m_Angles.x = 0.0f;
    m_Angles.y = 0.0f;
    m_Angles.z = 0.0f;
  } else {
    float gyr_x = m_Angles.x + (m_LastGyro.x + gx) / 2 * delta / 1e6;
    float gyr_y = m_Angles.y + (m_LastGyro.y + gy) / 2 * delta / 1e6;
    float gyr_z = m_Angles.z + (m_LastGyro.z + gz) / 2 * delta / 1e6;

    m_Angles.x = gyr_x;
    m_Angles.y = gyr_y;
    m_Angles.z = gyr_z;
  }

  m_LastTimestamp = rep->timestamp;
  m_LastGyro.x = gx;
  m_LastGyro.y = gy;
  m_LastGyro.z = gz;
  m_LastDelta = delta;
#endif

#ifdef INVERT_GYRO_X
  rep->gyroX = (int32_t)m_Calib[0].bias * 2 - rep->gyroX;
#endif
#ifdef INVERT_GYRO_Y
  rep->gyroY = (int32_t)m_Calib[1].bias * 2 - rep->gyroY;
#endif
#ifdef INVERT_GYRO_Z
  rep->gyroZ = (int32_t)m_Calib[2].bias * 2 - rep->gyroZ;
#endif
}
