/**
 * @file SDPClientFactory.cpp
 */

/**********************************************************************

  Created: 25 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include "SDPClientFactory.h"
#include "SonyControllerClientFactory.h"

namespace dsremap
{
  SDPClientFactory::SDPClientFactory()
    : BluetoothAcceptor::ClientFactory(),
      Logger("SDPClientFactory")
  {
  }

  SDPClientFactory::~SDPClientFactory()
  {
  }

  bool SDPClientFactory::on_new_connection(BluetoothAcceptor& acceptor, const std::string& addr, uint16_t psm, int fd)
  {
    if (psm == 0x01) {
      info("Got SDP connection; adding client");

      new SonyControllerClientFactory(acceptor, addr, fd);

      return true;
    }

    return false;
  }
}
