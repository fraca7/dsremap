/**
 * @file Proxy.cpp
 */

/**********************************************************************

  Created: 09 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <system_error>

#include <unistd.h>
#include <glib-unix.h>
#include <zlib.h>

#include "Proxy.h"

namespace dsremap
{
  Proxy::Proxy(Application& app, int fd_0x11, int fd_0x13)
    : Logger("Proxy"),
      Application::Component(app),
      _fd_0x11(fd_0x11),
      _fd_0x13(fd_0x13)
  {
    g_unix_fd_add(_fd_0x11, G_IO_IN, &Proxy::static_io_callback, static_cast<gpointer>(this));
    g_unix_fd_add(_fd_0x13, G_IO_IN, &Proxy::static_io_callback, static_cast<gpointer>(this));
  }

  Proxy::~Proxy()
  {
    while (g_source_remove_by_user_data(static_cast<gpointer>(this)));

    ::close(_fd_0x11);
    ::close(_fd_0x13);
  }

  void Proxy::get_report(uint8_t id, uint16_t len)
  {
    uint8_t data[4] = { 0x4b, 0x00, 0x00, 0x00 };
    data[1] = id;
    *((uint16_t*)(data + 2)) = len;
    if (::write(_fd_0x11, data, sizeof(data)) != sizeof(data))
      throw std::system_error(errno, std::generic_category(), "Error sending GET_REPORT");
  }

  void Proxy::write_out_data(std::vector<uint8_t>& data)
  {
    write_data(_fd_0x13, data);
  }

  void Proxy::write_int_data(std::vector<uint8_t>& data)
  {
    write_data(_fd_0x11, data);
  }

  void Proxy::write_data(int fd, std::vector<uint8_t>& data)
  {
    uint32_t crc = crc32(0x00000000, data.data(), data.size() - 4);
    *((uint32_t*)(data.data() + data.size() - 4)) = crc;

    if (::write(fd, data.data(), data.size()) < 0)
      throw std::system_error(errno, std::generic_category(), "Write error");
  }

  gboolean Proxy::static_io_callback(int fd, GIOCondition, gpointer ptr)
  {
    return static_cast<Proxy*>(ptr)->io_callback(fd);
  }

  bool Proxy::io_callback(int fd)
  {
    uint16_t psm = (fd == _fd_0x11) ? 0x11 : 0x13;

    std::vector<uint8_t> bf(4096);
    size_t len;
    if ((len = ::read(fd, bf.data(), 4096)) < 0) {
      error("Read error: {}", ::strerror(errno));
      stop();
      return false;
    }
    bf.resize(len);

    try {
      switch (psm) {
        case 0x11:
          got_control_data(bf);
          break;
        case 0x13:
          got_interrupt_data(bf);
          break;
      }
    } catch (const std::exception& exc) {
      error("On endpoint data: {}", exc.what());
      stop();
      return false;
    }

    return true;
  }
}
