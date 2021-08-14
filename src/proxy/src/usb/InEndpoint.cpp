/**
 * @file InEndpoint.cpp
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <unistd.h>

#include <system_error>

#include "USBDevice.h"
#include "InEndpoint.h"

namespace dsremap
{
  gsize InEndpoint::write(const uint8_t* data, gsize len)
  {
    int written = ::write(fd(), data, len);

    if (written < 0)
      throw std::system_error(errno, std::generic_category(), "Write error on endpoint");

    return written;
  }

  void InEndpoint::shutdown()
  {
    device().on_endpoint_shutdown(this);
  }
}
