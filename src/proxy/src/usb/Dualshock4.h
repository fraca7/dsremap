
/**
 * @file Dualshock4.h
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSHOCK4_H
#define _DUALSHOCK4_H

#include <src/usb/USBDevice.h>
#include <src/usb/DummyAudioInterface.h>
#include <src/usb/Dualshock4HIDInterface.h>

namespace dsremap
{
  class Dualshock4 : public USBDevice
  {
  public:
    Dualshock4(USBDevice::Listener&, HIDInterface::Listener&);
    virtual ~Dualshock4();

    Dualshock4HIDInterface& hid_interface() {
      return _hid_if;
    }

    const std::vector<uint8_t>& get_descriptor() const override;

    std::string name() const override {
      return "dualshock";
    }

    std::string manufacturer() const override {
      return "Sony Interactive Entertainment";
    }

    std::string product() const override {
      return "Wireless Controller";
    }

    uint16_t max_packet_size() const override {
      return 0x40;
    }

    uint16_t vendor_id() const override {
      return 0x54c;
    }

    uint16_t product_id() const override {
      return 0x09cc;
    }

  protected:
    Interface* get_interface(const Interface::interface_descriptor_t& desc) override;

  private:
    DummyAudioInterface _audio_if_0;
    DummyAudioInterface _audio_if_1;
    DummyAudioInterface _audio_if_2;
    Dualshock4HIDInterface _hid_if;

    static std::vector<uint8_t> _usb_descriptor;
  };
}

#endif /* _DUALSHOCK4_H */
