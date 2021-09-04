
#ifndef _DS4USB_H
#define _DS4USB_H

#include "Config.h"

#include <hiduniversal.h>

class Host;

class DS4USB : public HIDUniversal
{
public:
  DS4USB(USB*, Host*);

  // DS4 impl
  void GET_REPORT(uint8_t, uint8_t, uint16_t);
  void SET_REPORT(uint8_t, uint8_t, uint16_t, const uint8_t*);
  void SendData(uint16_t, const uint8_t*);

  // Endpoint extractor, to find out the HID interface number
  void EndpointXtract(uint8_t conf, uint8_t iface, uint8_t alt, uint8_t proto, const USB_ENDPOINT_DESCRIPTOR *pep) override;

protected:
  // HIDUniversal impl
  uint8_t OnInitSuccessful() override;
  void ParseHIDData(USBHID*, bool, uint8_t, uint8_t*) override;

private:
  Host* m_pHost;

  uint8_t m_Interface;
  bool m_PendingCalib;
};

#endif /* _DS4USB_H */
