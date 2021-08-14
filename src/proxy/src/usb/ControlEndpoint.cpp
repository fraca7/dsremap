/**
 * @file ControlEndpoint.cpp
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <system_error>

#include <glib-unix.h>

#include "USBDevice.h"
#include "ControlEndpoint.h"

namespace dsremap
{
  ControlEndpoint::ControlEndpoint(USBDevice& dev, int fd)
    : InEndpoint(dev, nullptr, 0, fd),
      _listener(dev)
  {
    g_unix_fd_add(fd, G_IO_IN, &ControlEndpoint::static_io_callback, static_cast<gpointer>(this));
  }

  ControlEndpoint::~ControlEndpoint()
  {
    while (g_source_remove_by_user_data(static_cast<gpointer>(this)));
  }

  gsize ControlEndpoint::read(uint8_t* ptr, gsize len)
  {
    ssize_t count = ::read(fd(), ptr, len);

    if (count < 0)
      throw std::system_error(errno, std::generic_category(), "Read error on control endpoint");

    return count;
  }

  bool ControlEndpoint::io_callback(GIOCondition cond)
  {
    try {
      _listener.on_control_data_available(*this);
    } catch (...) {
      _listener.on_error(std::current_exception());
      return false;
    }

    return true;
  }

  gboolean ControlEndpoint::static_io_callback(int, GIOCondition cond, gpointer ptr)
  {
    return static_cast<ControlEndpoint*>(ptr)->io_callback(cond);
  }
}
