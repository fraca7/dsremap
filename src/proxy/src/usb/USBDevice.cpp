/**
 * @file USBDevice.cpp
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <functional>
#include <fstream>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#include <src/utils/format.h>
#include <src/utils/File.h>

#include "DescriptorParser.h"
#include "ControlEndpoint.h"
#include "USBDevice.h"

namespace dsremap
{
  class InterfaceDescriptorParser : public DescriptorParser,
                                    public Logger
  {
  public:
    InterfaceDescriptorParser(USBDevice& dev)
      : DescriptorParser(dev),
        Logger("InterfaceDescriptorParser") {
      _dev._interfaces.clear();
    }

    Interface* on_interface(const Interface::interface_descriptor_t& descriptor) override {
      debug("New interface; class=0x{:02x} subclass=0x{:02x}", descriptor.bInterfaceClass, descriptor.bInterfaceSubClass);
      Interface* intf = _dev.get_interface(descriptor);
      _dev._interfaces.push_back(intf);
      return intf;
    }

    void on_endpoint(Interface*, unsigned int, uint8_t) override {
    }

    void on_class_descriptor(Interface* intf, const std::vector<uint8_t>& descriptor) override {
      debug("Class-specific descriptor 0x{:02x}", descriptor[1]);
      intf->parse_usb_descriptor(descriptor);
    }
  };

  class EndpointDescriptorParser : public DescriptorParser,
                                   public Logger
  {
  public:
    EndpointDescriptorParser(USBDevice& dev)
      : DescriptorParser(dev),
        Logger("EndpointDescriptorParser") {
    }

  protected:
    Interface* on_interface(const Interface::interface_descriptor_t& descriptor) override {
      return _dev.get_interface(descriptor);
    }

    void on_endpoint(Interface* intf, unsigned int global_index, uint8_t index) override {
      File path(_dev._functionfs_helper->functionfs_path, format("ep{}", global_index));

      debug("Adding endpoint {}", path.as_string());

      if ((index & 0b10000000) == 0) {
        debug("Add OUT endpoint");

        int fd = ::open(path.as_string().c_str(), O_RDONLY|O_NONBLOCK);
        if (fd < 0)
          throw std::runtime_error(format("Cannot open endpoint {}: {}", global_index, ::strerror(errno)));

        OutEndpoint* ep = new OutEndpoint(_dev, intf, index & 0x0F, fd, _dev.max_packet_size());
        _dev._endpoints.push_back(std::unique_ptr<Endpoint>(ep));
        intf->add_out_endpoint(ep);
      } else {
        debug("Add IN endpoint");

        int fd = ::open(path.as_string().c_str(), O_WRONLY|O_NONBLOCK);
        if (fd < 0)
          throw std::runtime_error(format("Cannot open endpoint {}: {}", global_index, ::strerror(errno)));

        InEndpoint* ep = new InEndpoint(_dev, intf, index & 0x0F, fd);
        _dev._endpoints.push_back(std::unique_ptr<Endpoint>(ep));
        intf->add_in_endpoint(ep);
      }
    }

    void on_class_descriptor(Interface*, const std::vector<uint8_t>&) override {
    }
  };

  USBDevice::USBDevice(Listener& listener)
    : ControlEndpoint::Listener(),
      Logger("USBDevice"),
      _state(State::Detached),
      _listener(listener),
      _functionfs_helper(nullptr),
      _interfaces(),
      _endpoints()
  {
  }

  USBDevice::~USBDevice()
  {
  }

  void USBDevice::attach()
  {
    if (state() != State::Detached)
      throw std::runtime_error("Device already attached");

    _functionfs_helper.reset(new FunctionFS(*this));

    debug("Open ep0");

    int ep0_fd = ::open(File(_functionfs_helper->functionfs_path, "ep0").as_string().c_str(), O_RDWR|O_NONBLOCK);
    if (ep0_fd < 0)
      throw std::system_error(errno, std::generic_category(), "Cannot open ep0");

    try {
      struct {
        struct usb_functionfs_descs_head_v2 header;
        __le32 fs_count;
        __le32 hs_count;
      } __attribute__((packed)) h_desc = {
        .header = {
          .magic = FUNCTIONFS_DESCRIPTORS_MAGIC_V2,
          .flags = FUNCTIONFS_HAS_HS_DESC|FUNCTIONFS_HAS_FS_DESC,
        },
      };

      std::vector<uint8_t> d_desc = get_descriptor();
      h_desc.header.length = sizeof(h_desc) + 2 * d_desc.size();

      {
        h_desc.fs_count = h_desc.hs_count = 0;
        auto pos = d_desc.begin();
        while (pos != d_desc.end()) {
          ++h_desc.fs_count;
          ++h_desc.hs_count;
          pos += *pos;
        }
      }

      // Of course the driver needs everything to be sent as one buffer
      std::vector<uint8_t> data;
      data.insert(data.end(), (uint8_t*)&h_desc, (uint8_t*)&h_desc + sizeof(h_desc));
      data.insert(data.end(), d_desc.begin(), d_desc.end());
      data.insert(data.end(), d_desc.begin(), d_desc.end());

      debug("Send {} descriptor bytes", data.size());

      if (::write(ep0_fd, data.data(), data.size()) < 0)
        throw std::system_error(errno, std::generic_category(), "Cannot write descriptors");

      struct {
        struct usb_functionfs_strings_head header;
      } __attribute__((packed)) strings = {
        .header = {
          .magic = FUNCTIONFS_STRINGS_MAGIC,
          .length = sizeof(strings),
          .str_count = 0,
          .lang_count = 0,
        },
      };

      debug("Send {} strings bytes", sizeof(strings));

      if (::write(ep0_fd, &strings, sizeof(strings)) < 0)
        throw std::system_error(errno, std::generic_category(), "Cannot write strings");

      debug("Activating UDC");
      File(_functionfs_helper->gadget_path, "UDC").echo(_functionfs_helper->udc.as_string());
    } catch (...) {
      debug("Closing ep0");
      ::close(ep0_fd);
      throw;
    }

    _ep0.reset(new ControlEndpoint(*this, ep0_fd));
  }

  void USBDevice::detach()
  {
    if (state() != State::Attached)
      throw std::runtime_error("Already detached");

    debug("Closing ep0");
    _ep0.reset(nullptr);

    if (_endpoints.size() == 0) {
      debug("No endpoints open; detaching immediately");

      _functionfs_helper.reset(nullptr);
      _state = State::Detached;
      _listener.on_device_detached(*this);
    } else {
      debug("Endpoints open; shutting them down");

      // The list may be changed while iterating
      std::list<Endpoint*> endpoints;
      for (auto& ptr : _endpoints)
        endpoints.push_back(ptr.get());
      for (auto& ep : endpoints)
        ep->shutdown();
    }
  }

  void USBDevice::on_endpoint_shutdown(Endpoint* ep)
  {
    debug("Endpoint shutdown");
    _endpoints.remove_if([ep](const std::unique_ptr<Endpoint>& existing) { return existing.get() == ep; });

    if (_endpoints.size() == 0) {
      debug("No more endpoints; detaching");
      _functionfs_helper.reset(nullptr);
      _state = State::Detached;
      _listener.on_device_detached(*this);
    } else {
      debug("{} more endpoints to go", _endpoints.size());
    }
  }

  void USBDevice::on_control_data_available(ControlEndpoint& ep)
  {
    usb_functionfs_event events[4];
    size_t len = ep.read((uint8_t*)&events, sizeof(events));

    for (unsigned int i = 0; i < len / sizeof(*events); ++i) {
      switch (events[i].type) {
        case FUNCTIONFS_BIND:
        {
          info("Bind event");
          InterfaceDescriptorParser parser(*this);
          parser.parse(get_descriptor());
          break;
        }
        case FUNCTIONFS_UNBIND:
          info("Unbind event");
          break;
        case FUNCTIONFS_ENABLE:
        {
          info("Enable event");
          EndpointDescriptorParser parser(*this);
          parser.parse(get_descriptor());
          _state = State::Attached;
          _listener.on_device_attached(*this);
          break;
        }
        case FUNCTIONFS_DISABLE:
          info("Disable event");
          break;
        case FUNCTIONFS_SUSPEND:
          info("Suspend event");
          break;
        case FUNCTIONFS_RESUME:
          info("Resume event");
          break;
        case FUNCTIONFS_SETUP:
          handle_setup(ep, events[i].u.setup);
          break;
        default:
          error("Unknown event type 0x{:02x}", events[i].type);
          break;
      }
    }
  }

  void USBDevice::on_error(std::exception_ptr exc_ptr)
  {
    _listener.on_error(exc_ptr);
  }

  void USBDevice::handle_setup(ControlEndpoint& ep, const struct usb_ctrlrequest& req)
  {
    debug("Setup event: bRequestType=0b{:08b}; bRequest=0x{:02x}; wValue=0x{:04x}; wIndex=0x{:04x}, wLength=0x{:04x}",
          req.bRequestType, req.bRequest, req.wValue, req.wIndex, req.wLength);

    switch (req.bRequestType & 0b1111) { // Recipient
      case 1: // Interface
      {
        Interface* intf = nullptr;
        for (auto& my_intf : _interfaces) {
          if (my_intf->index() == (req.wIndex & 0xFF)) {
            intf = my_intf;
            break;
          }
        }

        if (!intf) {
          error("Could not find interface #{}", (req.wIndex & 0xFF));
          return;
        }

        switch ((req.bRequestType >> 5) & 0b11) { // Type
          case 0: // Standard
            intf->handle_standard_request(ep, req);
            break;
          case 1: // Class
            intf->handle_class_request(ep, req);
            break;
          default:
            error("Ignored request for type 0x{:02x}", (req.bRequestType >> 5) & 0b11);
            break;
        }
        break;
      }
      default:
        error("Ignored request for recipient 0x{:02x}", req.bRequestType & 0b1111);
        break;
    }
  }
}
