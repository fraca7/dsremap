
/**
 * @file Interface.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _INTERFACE_H
#define _INTERFACE_H

#include <linux/usb/functionfs.h>

#include <cstdint>
#include <vector>
#include <memory>

#include <src/utils/Logger.h>
#include <src/usb/OutEndpoint.h>

namespace dsremap
{
  class InEndpoint;
  class ControlEndpoint;

  class Interface : public Logger,
                    public OutEndpoint::Listener
  {
  public:
    struct interface_descriptor_t {
      uint8_t bInterfaceNumber;
      uint8_t bAlternateSetting;
      uint8_t bNumEndpoints;
      uint8_t bInterfaceClass;
      uint8_t bInterfaceSubClass;
      uint8_t bInterfaceProtocol;
      uint8_t iInterface;
    } __attribute__((packed));

    Interface(unsigned int index);
    virtual ~Interface();

    virtual void add_in_endpoint(InEndpoint*) = 0;
    virtual void add_out_endpoint(OutEndpoint*) = 0;

    virtual void parse_usb_descriptor(const std::vector<uint8_t>&) = 0;
    virtual void handle_standard_request(ControlEndpoint&, const struct usb_ctrlrequest&) = 0;
    virtual void handle_class_request(ControlEndpoint&, const struct usb_ctrlrequest&) = 0;

    unsigned int index() const {
      return _index;
    }

  private:
    unsigned int _index;
  };
}

#endif /* _INTERFACE_H */
