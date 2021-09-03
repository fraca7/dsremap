
/**
 * @file Dualsense.h
 */

/**********************************************************************

  Created: 01 Sep 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSENSE_H
#define _DUALSENSE_H

#include <src/usb/USBDevice.h>
#include <src/usb/DummyAudioInterface.h>
#include <src/usb/DualsenseHIDInterface.h>

namespace dsremap
{
  class Dualsense : public USBDevice
  {
  public:
    Dualsense(USBDevice::Listener&, HIDInterface::Listener&);
    ~Dualsense();

    DualsenseHIDInterface& hid_interface() {
      return _hid_if;
    }

    const std::vector<uint8_t>& get_descriptor() const override;

    std::string name() const override {
      // For some reason if I name this "dualsense" the functionfs mount fails...
      return "sony-dualsense";
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
      return 0x0ce6;
    }

  protected:
    Interface* get_interface(const Interface::interface_descriptor_t& desc) override;

  private:
    DummyAudioInterface _audio_if_0;
    DummyAudioInterface _audio_if_1;
    DummyAudioInterface _audio_if_2;
    DualsenseHIDInterface _hid_if;

    static std::vector<uint8_t> _usb_descriptor;
  };
}

#endif /* _DUALSENSE_H */
