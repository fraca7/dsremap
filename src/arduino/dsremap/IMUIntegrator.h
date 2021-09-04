
#ifndef _IMUINTEGRATOR_H
#define _IMUINTEGRATOR_H

#ifndef TARGET_PC
#include "Config.h"
#endif

#include "DS4Structs.h"

class IMUIntegrator
{
public:
  IMUIntegrator();

  void SetCalibrationData(const CalibrationData_t*);
  void Update(const imu_state_t*);

  const Vector3D& CurrentAngles() const {
    return m_Angles;
  }

  const Vector3D& CurrentAnglesAccel() const {
    return m_LastGyro;
  }

  int32_t Delta() const {
    return m_LastDelta;
  }

private:
  typedef struct {
    int16_t bias;
    int32_t N;
    int32_t D;
  } Calibration_t;

  Calibration_t m_Calib[6];
  uint16_t m_LastTimestamp;
  bool m_FirstTimestamp;
  Vector3D m_Angles;
  Vector3D m_LastGyro;
  int32_t m_LastDelta;
};

#endif /* _IMUINTEGRATOR_H */
