
/**
 * @file Endpoint.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _ENDPOINT_H
#define _ENDPOINT_H

#include <glib.h>

namespace dsremap
{
  class Interface;
  class USBDevice;

  class Endpoint
  {
  public:
    Endpoint(USBDevice& dev, Interface* intf, unsigned int index, int fd);
    virtual ~Endpoint();

    Interface* interface() const {
      return _intf;
    }

    unsigned int index() const {
      return _index;
    }

    virtual void shutdown() = 0;

  protected:
    int fd() const {
      return _fd;
    }

    USBDevice& device() {
      return _dev;
    }

  private:
    USBDevice& _dev;
    Interface* _intf;
    unsigned int _index;
    int _fd;
  };
}

#endif /* _ENDPOINT_H */
