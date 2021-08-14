/**
 * @file BTDevice.cpp
 */

/**********************************************************************

  Created: 06 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <unistd.h>
#include <glib-unix.h>

#include <system_error>

#include <src/utils/format.h>

#include "BTUtils.h"
#include "BTDevice.h"

static const uint8_t sdp_pdu_request[] = {
  0x06, 0x00, 0x01, 0x00, 0x0f, 0x35, 0x03, 0x19, 0x01, 0x00, 0x08, 0x00, 0x35, 0x05, 0x0a, 0x00, 0x00, 0xff, 0xff, 0x00
};

namespace dsremap
{
  BTDevice::BTDevice(Listener& listener, const bdaddr_t* addr)
    : Logger("BTClient"),
      _listener(listener),
      _fds({ -1, -1, -1 }),
      _connected({ false, false, false })
  {
    memcpy(&_host_addr, addr, sizeof(*addr));
    _connect(0);
  }

  BTDevice::~BTDevice()
  {
    for (size_t i = 0; i < 3; ++i) {
      if (_fds[i] != -1)
        ::close(_fds[i]);
    }

    while (g_source_remove_by_user_data(static_cast<gpointer>(this)));
  }

  gboolean BTDevice::static_io_callback(int fd, GIOCondition cond, gpointer ptr)
  {
    return static_cast<BTDevice*>(ptr)->io_callback(fd, cond);
  }

  bool BTDevice::io_callback(int fd, GIOCondition cond)
  {
    try {
      uint16_t psm = 0x00;
      int index;
      for (index = 0; index < 3; ++index) {
        if (_fds[index] == fd) {
          switch (index) {
            case 0:
              psm = 0x01;
              break;
            case 1:
              psm = 0x11;
              break;
            case 2:
              psm = 0x13;
              break;
          }
          break;
        }
      }

      switch (cond) {
        case G_IO_OUT:
        {
          int err = 0;
          socklen_t len = sizeof(err);
          if ((::getsockopt(_fds[index], SOL_SOCKET, SO_ERROR, &err, &len) < 0) || err)
            throw std::system_error(errno, std::generic_category(), format("Connection error on PSM 0x{:02x}", psm));

          debug("Connection successful on PSM 0x{:02x}", psm);
          _connected[index] = true;

          if ((psm == 0x01) && (::write(_fds[0], sdp_pdu_request, sizeof(sdp_pdu_request)) < 0))
            throw std::system_error(errno, std::generic_category(), "Write error on SDP channel");

          g_unix_fd_add(_fds[index], G_IO_IN, &BTDevice::static_io_callback, static_cast<gpointer>(this));

          if (_connected[1] && _connected[2])
            _listener.on_bt_connected();

          return false;
        }
        case G_IO_IN:
        {
          std::vector<uint8_t> bf(4096);
          int len;

          if ((len = ::read(fd, bf.data(), bf.size())) < 0)
            throw std::system_error(errno, std::generic_category(), format("Read error on PSM 0x{:02x}", psm));

          bf.resize(len);

          switch (psm) {
            case 0x01:
              _connect(1);
              _connect(2);

              debug("Closing SDP channel");

              ::close(_fds[0]);
              _fds[0] = -1;
              return false;
            case 0x11:
              switch ((bf[0] & 0xF0) >> 4) {
                case 0x04:
                  _listener.on_bt_get_report(fd, bf[0] & 0b11, bf[1]);
                  break;
                case 0x05:
                  _listener.on_bt_set_report(bf[0] & 0b11, bf[1], bf);
                  break;
                case 0x0a:
                  warn("Ignoring DATA buffer on BTHID control pipe");
                  break;
                default:
                  warn("Unknown command 0x{:02x} on BTHID control pipe", bf[0]);
              }
              break;
            case 0x13:
              _listener.on_bt_out_report(bf[1], bf);
              break;
          }
          break;
        }
        default:
          break;
      }
    } catch (...) {
      _listener.on_error(std::current_exception());
      return false;
    }

    return true;
  }

  void BTDevice::_connect(int index)
  {
    uint16_t psm = 0x00;
    switch (index) {
      case 0:
        psm = 0x01;
        break;
      case 1:
        psm = 0x11;
        break;
      case 2:
        psm = 0x13;
        break;
    }

    if ((_fds[index] = socket(PF_BLUETOOTH, SOCK_SEQPACKET|SOCK_NONBLOCK, BTPROTO_L2CAP)) < 0)
      throw std::system_error(errno, std::generic_category(), format("Cannot create client socket for PSM 0x{:02x}", psm));

    try {
      set_l2cap_socket_options(_fds[index]);

      struct sockaddr_l2 l2addr;
      memset(&l2addr, 0, sizeof(l2addr));
      l2addr.l2_family = AF_BLUETOOTH;
      HCIDevice(0).get_bdaddr(&l2addr.l2_bdaddr);
      if (::bind(_fds[index], (struct sockaddr*)&l2addr, sizeof(l2addr)) < 0)
        throw std::system_error(errno, std::generic_category(), format("Cannot bind client socket for PSM 0x{:02x}", psm));

      memset(&l2addr, 0, sizeof(l2addr));
      l2addr.l2_family = AF_BLUETOOTH;
      l2addr.l2_psm = htobs(psm);
      memcpy(&l2addr.l2_bdaddr, &_host_addr, sizeof(_host_addr));

      if (::connect(_fds[index], (struct sockaddr*)&l2addr, sizeof(l2addr)) < 0) {
        switch (errno) {
          case EINPROGRESS:
            break;
          default:
            throw std::system_error(errno, std::generic_category(), format("Error connecting client socket for PSM 0x{:02x}", psm));
        }
      }

      g_unix_fd_add(_fds[index], G_IO_OUT, &BTDevice::static_io_callback, static_cast<gpointer>(this));
    } catch (...) {
      ::close(_fds[index]);
      _fds[index] = -1;
      throw;
    }

  }
}
