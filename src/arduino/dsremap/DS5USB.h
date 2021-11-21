
#ifndef _DS5USB_H
#define _DS5USB_H

#include "USBController.h"

class DS5USB : public USBController
{
public:
  DS5USB(USB*, Host*);

  void ControllerStateFromBuffer(controller_state_t*, const uint8_t*) override;
  void ControllerStateToBuffer(const controller_state_t*, uint8_t*) override;
  void IMUStateFromBuffer(imu_state_t*, const uint8_t*) override;

protected:
  void GetCalibrationData() override;
};

#endif /* _DS5USB_H */
