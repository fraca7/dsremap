/**
 * @file BluetoothAcceptor.cpp
 */

/**********************************************************************

  Created: 25 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <system_error>

#include <glib-unix.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>

#include <src/utils/format.h>

#include "BTUtils.h"
#include "BluetoothAcceptor.h"
#include "SDPClientFactory.h"

namespace dsremap
{
  BluetoothAcceptor::BluetoothAcceptor()
    : Logger("BluetoothAcceptor"),
      Application(),
      _descriptors{{-1, 0x01}, {-1, 0x11}, {-1, 0x13}},
      _factory(nullptr),
      _client_factories()
  {
    try {
      for (auto& dsc : _descriptors) {
        if ((dsc.fd = ::socket(AF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP)) < 0)
          throw std::system_error(errno, std::generic_category(), format("Cannot create socket for PSM 0x{:02x}", dsc.psm));

        set_l2cap_socket_options(dsc.fd);

        struct sockaddr_l2 addr;
        memset(&addr, 0, sizeof(addr));
        addr.l2_family = AF_BLUETOOTH;
        addr.l2_psm = htobs(dsc.psm);
        if (::bind(dsc.fd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
          throw std::system_error(errno, std::generic_category(), format("Cannot bind socket for PSM 0x{:02x}", dsc.psm));

        if (::listen(dsc.fd, 5) < 0)
          throw std::system_error(errno, std::generic_category(), format("Cannot listen on PSM 0x{:02x}", dsc.psm));

        g_unix_fd_add(dsc.fd, G_IO_IN, &BluetoothAcceptor::static_accept_callback, static_cast<gpointer>(this));

        info("Listening on PSM 0x{:02x}", dsc.psm);
      }
    } catch (...) {
      for (const auto& dsc : _descriptors) {
        if (dsc.fd != -1) {
          ::close(dsc.fd);
        }
      }

      while (g_source_remove_by_user_data(static_cast<gpointer>(this)));

      throw;
    }

    _factory = new SDPClientFactory();
    add_client_factory(_factory);
  }

  BluetoothAcceptor::~BluetoothAcceptor()
  {
    for (const auto& dsc : _descriptors)
      ::close(dsc.fd);
    while (g_source_remove_by_user_data(static_cast<gpointer>(this)));

    delete _factory;
  }

  void BluetoothAcceptor::add_client_factory(ClientFactory* factory)
  {
    _client_factories.push_front(factory);
  }

  void BluetoothAcceptor::remove_client_factory(ClientFactory* factory)
  {
    _client_factories.remove(factory);
  }

  gboolean BluetoothAcceptor::static_accept_callback(gint fd, GIOCondition, gpointer ptr)
  {
    return static_cast<BluetoothAcceptor*>(ptr)->accept_callback(fd);
  }

  bool BluetoothAcceptor::accept_callback(int fd)
  {
    if (state() == State::Stopping)
      return false;

    for (const auto& dsc : _descriptors) {
      if (dsc.fd == fd) {
        struct sockaddr_l2 addr;
        memset(&addr, 0, sizeof(addr));
        socklen_t len = sizeof(addr);
        int new_fd = ::accept(fd, (struct sockaddr*)&addr, &len);
        if (new_fd < 0) {
          error("Cannot accept new connection on PSM 0x{:02x}: {}", dsc.psm, ::strerror(errno));
          return true;
        }

        int flags = ::fcntl(new_fd, F_GETFL, 0);
        ::fcntl(new_fd, F_SETFL, flags|O_NONBLOCK);

        char buf[18];
        ::ba2str(&addr.l2_bdaddr, buf);
        info("Accepted new connection from {}; PSM 0x{:02x}", buf, dsc.psm);

        // add/remove may be called from the callback itself so copy
        std::list<ClientFactory*> factories(_client_factories.begin(), _client_factories.end());
        for (auto factory : factories) {
          if (factory->on_new_connection(*this, buf, dsc.psm, new_fd))
            return true;
        }

        warn("No handler found for incoming connection; closing");
        ::close(new_fd);

        break;
      }
    }

    return true;
  }
}
