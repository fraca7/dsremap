
/**
 * @file Dualshock4Proxy.h
 */

/**********************************************************************

  Created: 27 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSHOCK4PROXY_H
#define _DUALSHOCK4PROXY_H

#include <array>
#include <memory>

#include <glib.h>

#include <src/utils/Application.h>
#include <src/bluetooth/BTDevice.h>
#include <src/bluetooth/Proxy.h>
#include <src/usb/Dualshock4.h>

#include <src/AuthChallenge.h>

#include <dsremap/IMUIntegrator.h>
#include <dsremap/VM.h>

namespace dsremap
{
  /**
   * The class that ties all the stuff together. It starts by getting
   * some report values from the Dualshock, then attaches the USB
   * gadget, and connects to the PS4 through Bluetooth when asked.
   */
  class Dualshock4Proxy : public Proxy,
                          public USBDevice::Listener,
                          public HIDInterface::Listener,
                          public BTDevice::Listener
  {
  public:
    Dualshock4Proxy(Application&, int fd_0x11, int fd_0x13);
    ~Dualshock4Proxy();

    // USBDevice::Listener
    void on_device_attached(USBDevice&) override;
    void on_device_detached(USBDevice&) override;

    // HIDInterface::Listener
    void on_usb_get_report(ControlEndpoint&, int, int) override;
    void on_usb_set_report(int, int, const std::vector<uint8_t>&) override;
    void on_usb_out_report(int, const std::vector<uint8_t>&) override;

    // BTDevice::Listener
    void on_bt_connected() override;
    void on_bt_get_report(int, int, int) override;
    void on_bt_set_report(int, int, const std::vector<uint8_t>&) override;
    void on_bt_out_report(int, const std::vector<uint8_t>&) override;

    // Application::Component
    void stop() override;
    void reconfigure() override;

  private:
    enum class State {
      GatheringReports,
      Attaching,
      SendPS,
      Connecting,
      Running,
      Closing
    };
    State _state;
    unsigned int _pscount;

    GMainLoop* _loop;
    std::array<uint8_t, 54> _report_06;
    std::array<uint8_t, 50> _report_a3;
    std::array<uint8_t, 37> _report_02_usb;
    std::array<uint8_t, 38> _report_02_bt;
    std::array<uint8_t, 6> _host_addr;
    std::unique_ptr<Dualshock4> _usb_device;
    std::unique_ptr<BTDevice> _bt_device;

    AuthChallenge _auth;

    IMUIntegrator _imu;
    std::unique_ptr<VM> _vm;

    void got_control_data(const std::vector<uint8_t>&) override;
    void got_interrupt_data(const std::vector<uint8_t>&) override;

    void on_error(std::exception_ptr) override;

    static gboolean static_io_callback(int, GIOCondition, gpointer);
    bool io_callback(int);
  };
}

#endif /* _DUALSHOCK4PROXY_H */
