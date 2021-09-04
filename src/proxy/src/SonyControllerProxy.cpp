/**
 * @file SonyControllerProxy.cpp
 */

/**********************************************************************

  Created: 04 sept. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <cassert>

#include <src/utils/BytecodeFile.h>

#include "SonyControllerProxy.h"

namespace dsremap
{
  SonyControllerProxy::SonyControllerProxy(Application& app, int fd_0x11, int fd_0x13)
    : Proxy(app, fd_0x11, fd_0x13),
      USBDevice::Listener(),
      HIDInterface::Listener(),
      _state(State::GatheringReports),
      _imu(),
      _vm(nullptr),
      _bt_device(nullptr)
  {
    reconfigure();
  }

  void SonyControllerProxy::reconfigure()
  {
    _vm.reset(nullptr);

    BytecodeFile bc;
    if (bc.exists())
      _vm.reset(new VM(bc.bytecode(), true, bc.stacksize()));
  }

  void SonyControllerProxy::stop()
  {
    switch (_state) {
      case State::GatheringReports:
        info("Request stop in GatheringReports; stopping");
        delete this;
      case State::Attaching:
        info("Request stop in Attaching; closing");
        _state = State::Closing;
        break;
      case State::SendPS:
        info("Request stop in SendPS; closing");
        _state = State::Closing;
        usb_device().detach();
        break;
      case State::Connecting:
      case State::Running:
        info("Request stop in Running; closing");
        _bt_device.reset(nullptr);
        _state = State::Closing;
        usb_device().detach();
        break;
      case State::Closing:
        break;
    }
  }

  //==============================================================================
  // USB gadget housekeeping

  void SonyControllerProxy::on_device_attached(USBDevice&)
  {
    switch (_state) {
      case State::GatheringReports:
      case State::SendPS:
      case State::Connecting:
      case State::Running:
        assert(0);
        break;
      case State::Attaching:
        info("Device attached; moving to state Connecting");
        _state = State::Connecting;
        break;
      case State::Closing:
        info("Device attached; detaching immediately");
        usb_device().detach();
        break;
    }
  }

  void SonyControllerProxy::on_device_detached(USBDevice&)
  {
    info("Device detached; stopping");
    delete this;
  }

  //==============================================================================
  // Error handling

  void SonyControllerProxy::on_error(std::exception_ptr exc_ptr)
  {
    try {
      std::rethrow_exception(exc_ptr);
    } catch (const std::exception& exc) {
      error("Error: {}", exc.what());
    }

    stop();
  }
}
