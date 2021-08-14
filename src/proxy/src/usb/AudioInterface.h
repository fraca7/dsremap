
/**
 * @file AudioInterface.h
 */

/**********************************************************************

  Created: 26 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _AUDIOINTERFACE_H
#define _AUDIOINTERFACE_H

#include <src/usb/Interface.h>

namespace dsremap
{
  class AudioInterface : public Interface
  {
  public:
    using Interface::Interface;

    void parse_usb_descriptor(const std::vector<uint8_t>&) override;
    void handle_standard_request(ControlEndpoint&, const struct usb_ctrlrequest&) override;
    void handle_class_request(ControlEndpoint&, const struct usb_ctrlrequest&) override;

    virtual void get_cur(ControlEndpoint&, uint8_t id, uint16_t wValue) = 0;
    virtual void get_min(ControlEndpoint&, uint8_t id, uint16_t wValue) = 0;
    virtual void get_max(ControlEndpoint&, uint8_t id, uint16_t wValue) = 0;
    virtual void get_res(ControlEndpoint&, uint8_t id, uint16_t wValue) = 0;

    virtual void set_cur(uint8_t id, uint16_t wValue, uint16_t value) = 0;
  };
}

#endif /* _AUDIOINTERFACE_H */
