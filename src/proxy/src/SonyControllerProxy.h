
/**
 * @file SonyControllerProxy.h
 */

/**********************************************************************

  Created: 04 sept. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _SONYCONTROLLERPROXY_H
#define _SONYCONTROLLERPROXY_H

#include <src/utils/Application.h>
#include <src/usb/HIDInterface.h>
#include <src/usb/USBDevice.h>
#include <src/bluetooth/BTDevice.h>
#include <src/bluetooth/Proxy.h>

#include <dsremap/IMUIntegrator.h>
#include <dsremap/VM.h>

namespace dsremap
{
  /**
   * The class that ties all the stuff together. It starts by getting
   * some report values from the Dualshock, then attaches the USB
   * gadget, and connects to the Playstation through Bluetooth when asked.
   */
  class SonyControllerProxy : public Proxy,
                              public USBDevice::Listener,
                              public HIDInterface::Listener,
                              public BTDevice::Listener
  {
  public:
    SonyControllerProxy(Application&, int fd_0x11, int fd_0x13);

    // USBDevice::Listener
    void on_device_attached(USBDevice&) override;
    void on_device_detached(USBDevice&) override;

    // Application::Component
    void reconfigure() override;
    void stop() override;

  protected:
    virtual USBDevice& usb_device() = 0;

    enum class State {
      GatheringReports,
      Attaching,
      SendPS,
      Connecting,
      Running,
      Closing
    };
    State _state;

    IMUIntegrator _imu;
    std::unique_ptr<VM> _vm;
    std::unique_ptr<BTDevice> _bt_device;

    void on_error(std::exception_ptr) override;
  };
}

#endif /* _SONYCONTROLLERPROXY_H */
