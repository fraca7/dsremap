
/**
 * @file SonyController.h
 */

/**********************************************************************

  Created: 04 sept. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _SONYCONTROLLER_H
#define _SONYCONTROLLER_H

#include <stdexcept>

#include <src/usb/USBDevice.h>
#include <src/usb/HIDInterface.h>
#include <src/usb/DummyAudioInterface.h>

namespace dsremap
{
  template <typename HIDIF> class SonyController : public USBDevice
  {
  public:
    SonyController(USBDevice::Listener& dev_listener, HIDInterface::Listener& hid_listener)
      : USBDevice(dev_listener),
        _audio_if_0(0),
        _audio_if_1(1),
        _audio_if_2(2),
        _hid_if(hid_listener)
    {
    }

    HIDInterface& hid_interface()
    {
      return _hid_if;
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

  protected:
    Interface* get_interface(const Interface::interface_descriptor_t& desc)
    {
      switch (desc.bInterfaceClass) {
        case 0x03: // HID
          switch (desc.bInterfaceNumber) {
            case 3:
              return &_hid_if;
            default:
              throw std::runtime_error("Unknown HID interface");
          }
          break;
        case 0x01: // Audio
          switch (desc.bInterfaceNumber) {
            case 0:
              return &_audio_if_0;
            case 1:
              return &_audio_if_1;
            case 2:
              return &_audio_if_2;
            default:
              throw std::runtime_error("Unknown Audio interface");
          }
          break;
        default:
          throw std::runtime_error("Unknown interface class");
      }
    }

  private:
    DummyAudioInterface _audio_if_0;
    DummyAudioInterface _audio_if_1;
    DummyAudioInterface _audio_if_2;
    HIDIF _hid_if;
  };
}

#endif /* _SONYCONTROLLER_H */
