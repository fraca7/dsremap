
/**
 * @file Dualsense.h
 */

/**********************************************************************

  Created: 01 Sep 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSENSE_H
#define _DUALSENSE_H

#include <src/usb/SonyController.h>
#include <src/usb/DualsenseHIDInterface.h>

namespace dsremap
{
  class Dualsense : public SonyController<DualsenseHIDInterface>
  {
  public:
    Dualsense(USBDevice::Listener&, HIDInterface::Listener&);

    const std::vector<uint8_t>& get_descriptor() const override;

    std::string name() const override {
      // For some reason if I name this "dualsense" the functionfs mount fails...
      return "sony-dualsense";
    }

    uint16_t product_id() const override {
      return 0x0ce6;
    }

  private:
    static std::vector<uint8_t> _usb_descriptor;
  };
}

#endif /* _DUALSENSE_H */
