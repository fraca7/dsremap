
#ifndef _DS4USB_H
#define _DS4USB_H

#include "USBController.h"

class DS4USB : public USBController
{
public:
  DS4USB(USB*, Host*);

  void ControllerStateFromBuffer(controller_state_t*, const uint8_t*) override;
  void ControllerStateToBuffer(const controller_state_t*, uint8_t*) override;
  void IMUStateFromBuffer(imu_state_t*, const uint8_t*) override;
  void IMUStateToBuffer(const imu_state_t*, uint8_t*) override;

protected:
  void GetCalibrationData() override;
};

#endif /* _DS4USB_H */
