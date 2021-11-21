
#ifndef _USBCONTROLLER_H
#define _USBCONTROLLER_H

#include "Config.h"
#include "CommonStructs.h"

#include <hiduniversal.h>

class Host;

class USBController : public HIDUniversal
{
public:
  USBController(USB*, Host*);

  // DS4 impl
  void GET_REPORT(uint8_t, uint8_t, uint16_t);
  void SET_REPORT(uint8_t, uint8_t, uint16_t, const uint8_t*);
  void SendData(uint16_t, const uint8_t*);

  // Endpoint extractor, to find out the HID interface number
  void EndpointXtract(uint8_t conf, uint8_t iface, uint8_t alt, uint8_t proto, const USB_ENDPOINT_DESCRIPTOR *pep) override;

  virtual void ControllerStateFromBuffer(controller_state_t*, const uint8_t*) = 0;
  virtual void ControllerStateToBuffer(const controller_state_t*, uint8_t*) = 0;
  virtual void IMUStateFromBuffer(imu_state_t*, const uint8_t*) = 0;

protected:
  // HIDUniversal impl
  uint8_t OnInitSuccessful() override;
  void ParseHIDData(USBHID*, bool, uint8_t, uint8_t*) override;

  virtual void GetCalibrationData() = 0;

private:
  Host* m_pHost;

  uint8_t m_Interface;
  bool m_PendingCalib;
};

#endif /* _USBCONTROLLER_H */
