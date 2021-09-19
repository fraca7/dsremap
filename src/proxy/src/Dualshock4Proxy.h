
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

#include <src/usb/Dualshock4.h>

#include <src/SonyControllerProxy.h>
#include <src/AuthChallenge.h>

namespace dsremap
{
  class Dualshock4Proxy : public SonyControllerProxy
  {
  public:
    Dualshock4Proxy(BluetoothAcceptor&, int fd_0x11, int fd_0x13);

    // HIDInterface::Listener
    void on_usb_get_report(ControlEndpoint&, int, int) override;
    void on_usb_set_report(int, int, const std::vector<uint8_t>&) override;
    void on_usb_out_report(int, const std::vector<uint8_t>&) override;

    // BTDevice::Listener
    void on_bt_connected() override;
    void on_bt_get_report(int, int, int) override;
    void on_bt_set_report(int, int, const std::vector<uint8_t>&) override;
    void on_bt_out_report(int, const std::vector<uint8_t>&) override;
    const std::vector<uint8_t>& get_ssa_response() override {
      return _ssa_response;
    }

  protected:
    USBDevice& usb_device() override {
      return *_usb_device;
    }

  private:
    unsigned int _pscount;
    BluetoothAcceptor& _acceptor;

    std::array<uint8_t, 54> _report_06;
    std::array<uint8_t, 50> _report_a3;
    std::array<uint8_t, 37> _report_02_usb;
    std::array<uint8_t, 38> _report_02_bt;
    std::array<uint8_t, 6> _host_addr;
    std::unique_ptr<Dualshock4> _usb_device;

    AuthChallenge _auth;

    void got_control_data(const std::vector<uint8_t>&) override;
    void got_interrupt_data(const std::vector<uint8_t>&) override;

    static const std::vector<uint8_t> _ssa_response;
  };
}

#endif /* _DUALSHOCK4PROXY_H */
