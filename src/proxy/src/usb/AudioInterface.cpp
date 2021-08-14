/**
 * @file AudioInterface.cpp
 */

/**********************************************************************

  Created: 26 Jul 2021

    Copyright (C) 2021 Quividi

**********************************************************************/

#include "ControlEndpoint.h"
#include "AudioInterface.h"

namespace dsremap
{
  void AudioInterface::parse_usb_descriptor(const std::vector<uint8_t>& desc)
  {
    // Whatever
  }

  void AudioInterface::handle_standard_request(ControlEndpoint& ep, const struct usb_ctrlrequest& req)
  {
    warn("Ignored audio standard request");
  }

  void AudioInterface::handle_class_request(ControlEndpoint& ep, const struct usb_ctrlrequest& req)
  {
    switch (req.bRequestType >> 7) {
      case 0: // H -> D
        switch (req.bRequest) {
          case 0x01:
          {
            uint16_t value;
            ep.read((uint8_t*)&value, sizeof(value));
            set_cur(req.wIndex >> 8, req.wValue, value);
            break;
          }
          default:
            warn("Ignored class request");
        }
        break;
      case 1: // D -> H
      {
        switch (req.bRequest) {
          case 0x81:
            get_cur(ep, req.wIndex >> 8, req.wValue);
            break;
          case 0x82:
            get_min(ep, req.wIndex >> 8, req.wValue);
            break;
          case 0x83:
            get_max(ep, req.wIndex >> 8, req.wValue);
            break;
          case 0x84:
            get_res(ep, req.wIndex >> 8, req.wValue);
            break;
          default:
            warn("Ignored class request");
            break;
        }
        break;
      }
    }
  }
}
