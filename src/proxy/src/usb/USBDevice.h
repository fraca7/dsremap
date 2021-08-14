
/**
 * @file USBDevice.h
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _USBDEVICE_H
#define _USBDEVICE_H

#include <string>
#include <vector>
#include <list>
#include <cstdint>
#include <memory>
#include <stdexcept>

#include <glib.h>

#include <src/utils/Logger.h>
#include <src/utils/Application.h>
#include <src/usb/ControlEndpoint.h>
#include <src/usb/Interface.h>
#include <src/usb/FunctionFS.h>

namespace dsremap {
  class File;

  /**
   * The connection to the host, so something that behaves like an USB
   * device.
   */
  class USBDevice : public ControlEndpoint::Listener,
                    public Logger {
  public:
    enum class State {
      Attached,
      Detached
    };

    class Listener : public Application::ErrorHandler
    {
    public:
      virtual void on_device_attached(USBDevice&) = 0;
      virtual void on_device_detached(USBDevice&) = 0;
    };

    USBDevice(Listener&);
    virtual ~USBDevice();

    State state() const {
      return _state;
    }

    void attach();
    void detach();
    void on_endpoint_shutdown(Endpoint*);

    virtual const std::vector<uint8_t>& get_descriptor() const = 0;

    virtual std::string name() const = 0;
    virtual std::string manufacturer() const = 0;
    virtual std::string product() const = 0;
    virtual uint16_t max_packet_size() const = 0;
    virtual uint16_t vendor_id() const = 0;
    virtual uint16_t product_id() const = 0;

    // ControlEndpoint::Listener
    void on_control_data_available(ControlEndpoint&) override;

    // Application::ErrorHandler
    void on_error(std::exception_ptr) override;

  protected:
    virtual Interface* get_interface(const Interface::interface_descriptor_t& desc) = 0;

  private:
    State _state;
    Listener& _listener;

    void handle_setup(ControlEndpoint&, const struct usb_ctrlrequest&);

    std::unique_ptr<FunctionFS> _functionfs_helper;
    std::list<Interface*> _interfaces;
    std::unique_ptr<ControlEndpoint> _ep0;
    std::list<std::unique_ptr<Endpoint>> _endpoints;

    friend class InterfaceDescriptorParser;
    friend class EndpointDescriptorParser;
  };
}

#endif /* _USBDEVICE_H */
