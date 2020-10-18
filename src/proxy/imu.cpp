
#include <glib.h>

#include "imu.h"
#include "imu_private.h"

#include "cpp/IMUIntegrator.h"

struct imu_t* imu_init()
{
  struct imu_t* imu = (struct imu_t*)g_malloc(sizeof(struct imu_t));
  imu->ptr = new IMUIntegrator();
  return imu;
}

void imu_free(struct imu_t* imu)
{
  delete imu->ptr;
  g_free(imu);
}

void imu_set_calibration_data(struct imu_t* imu, const CalibrationData_t* data)
{
  imu->ptr->SetCalibrationData(data);
}

void imu_update(struct imu_t* imu, const BTReport11_t* report)
{
  imu->ptr->Update((const USBReport01_t*)((const uint8_t*)report + 2));
}
