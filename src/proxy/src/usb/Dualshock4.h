
/**
 * @file Dualshock4.h
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSHOCK4_H
#define _DUALSHOCK4_H

#include <src/usb/Dualshock4HIDInterface.h>
#include <src/usb/SonyController.h>

namespace dsremap
{
  class Dualshock4 : public SonyController<Dualshock4HIDInterface>
  {
  public:
    Dualshock4(USBDevice::Listener&, HIDInterface::Listener&);

    const std::vector<uint8_t>& get_descriptor() const override;

    std::string name() const override {
      return "dualshock";
    }

    uint16_t product_id() const override {
      return 0x09cc;
    }

  private:
    static std::vector<uint8_t> _usb_descriptor;
  };
}

#endif /* _DUALSHOCK4_H */
