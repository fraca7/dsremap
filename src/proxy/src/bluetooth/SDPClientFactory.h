
/**
 * @file SDPClientFactory.h
 */

/**********************************************************************

  Created: 25 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _SDPCLIENTFACTORY_H
#define _SDPCLIENTFACTORY_H

#include <src/utils/Logger.h>
#include <src/bluetooth/BluetoothAcceptor.h>

namespace dsremap
{
  /**
   * Standard acceptor client factory; creates a new
   * Dualshock4ClientFactory on incoming SDP connections.
   */
  class SDPClientFactory : public BluetoothAcceptor::ClientFactory,
                           public Logger
  {
  public:
    SDPClientFactory();
    ~SDPClientFactory();

    bool on_new_connection(BluetoothAcceptor& acceptor, const std::string& addr, uint16_t psm, int fd) override;
  };
}

#endif /* _SDPCLIENTFACTORY_H */
