/**
 * @file HIDInterface.cpp
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <src/utils/format.h>

#include "ControlEndpoint.h"
#include "HIDInterface.h"

namespace dsremap
{
  HIDInterface::HIDInterface(unsigned int index, HIDInterface::Listener& listener)
    : Interface(index),
      _listener(listener),
      _protocol(Protocol::Report),
      _data_enabled(false),
      _ep1(nullptr)
  {
#ifdef ENABLE_TIMING
    _last_timing = std::chrono::high_resolution_clock::now();
    _timing_count = 0U;
#endif
  }

  HIDInterface::~HIDInterface()
  {
  }

  void HIDInterface::enable_interrupt_in()
  {
    _data_enabled = true;
  }

  void HIDInterface::send_input_report(const uint8_t* ptr, uint16_t len)
  {
    if (_data_enabled) {
      _ep1->write(ptr, len);
#ifdef ENABLE_TIMING
      auto now = std::chrono::high_resolution_clock::now();
      if (now - _last_timing > std::chrono::seconds(1)) {
        if (_timing_count)
          debug("{} input/s", _timing_count);
        _timing_count = 0U;
        _last_timing = now;
      }

      ++_timing_count;
#endif
    }
  }

  void HIDInterface::add_in_endpoint(InEndpoint* ep)
  {
    _ep1 = ep;
  }

  void HIDInterface::add_out_endpoint(OutEndpoint* ep)
  {
    ep->set_enabled(true);
  }

  void HIDInterface::on_endpoint_data(OutEndpoint&, const std::vector<uint8_t>& data)
  {
    listener().on_usb_out_report(data[0], data);
  }

  void HIDInterface::on_error(std::exception_ptr exc_ptr)
  {
    listener().on_error(exc_ptr);
  }

  void HIDInterface::parse_usb_descriptor(const std::vector<uint8_t>& data)
  {
    switch (data[1]) {
      case 0x21: // HID
        break;
      default:
        throw std::runtime_error(format("Unknown HID-specific descriptor 0x{:02x}", data[1]));
    }
  }

  void HIDInterface::handle_standard_request(ControlEndpoint& ep, const struct usb_ctrlrequest& req)
  {
    switch (req.bRequestType >> 7) {
      case 0: // H -> D
      {
        error("Ignoring standard H -> D request");
        break;
      }
      case 1: // D -> H
      {
        switch (req.bRequest) {
          case 0x06:
            switch (req.wValue >> 8) {
              case 0x22:
              {
                auto report_descriptor = get_report_descriptor();

                info("Sending report descriptor ({} bytes)", report_descriptor.size());
                ep.write(report_descriptor.data(), report_descriptor.size());
                break;
              }
              default:
                error("Ignoring GET_DESCRIPTOR for 0x{:02x}", req.wValue >> 8);
            }
            break;
          default:
            error("Ignoring standard D->H request");
        }
        break;
      }
    }
  }

  void HIDInterface::handle_class_request(ControlEndpoint& ep, const struct usb_ctrlrequest& req)
  {
    switch (req.bRequestType >> 7) {
      case 0: // H -> D
      {
        std::vector<uint8_t> data;

        if (req.wLength) {
          // We assume the data is immediately available...
          data.resize(req.wLength);
          gsize len = ep.read(data.data(), req.wLength);
          data.resize(len);
        }

        switch (req.bRequest) {
          case 0x09:
            _listener.on_usb_set_report(req.wValue >> 8, req.wValue & 0xFF, data);
            break;
          case 0x0a:
            debug("SET_IDLE");
            // TODO maybe
            break;
          case 0x0b:
            debug("SET_PROTOCOL");
            switch (req.wValue) {
              case 0:
                set_protocol(Protocol::Boot);
                break;
              case 1:
                set_protocol(Protocol::Report);
                break;
            }
            break;
          default:
            error("Unhandled HID request 0x{:02x}", req.bRequest);
            break;
        }
        break;
      }
      case 1: // D -> H
      {
        switch (req.bRequest) {
          case 0x01:
            _listener.on_usb_get_report(ep, req.wValue >> 8, req.wValue & 0xFF);
            break;
          case 0x02:
            debug("GET_IDLE");
            // TODO maybe
            break;
          case 0x03:
          {
            debug("GET_PROTOCOL");
            uint8_t proto;
            switch (get_protocol()) {
              case Protocol::Boot:
                proto = 0;
                break;
              case Protocol::Report:
                proto = 1;
                break;
            }
            ep.write(&proto, 1);
            break;
          }
          default:
            error("Unhandled HID request 0x{:02x}", req.bRequest);
            break;
        }
        break;
      }
    }
  }
}
