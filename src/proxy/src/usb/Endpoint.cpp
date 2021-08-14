/**
 * @file Endpoint.cpp
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <unistd.h>

#include "Endpoint.h"

namespace dsremap
{
  Endpoint::Endpoint(USBDevice& dev, Interface* intf, unsigned int index, int fd)
    : _dev(dev),
      _intf(intf),
      _index(index),
      _fd(fd)
  {
  }

  Endpoint::~Endpoint()
  {
    ::close(_fd);
  }
}
