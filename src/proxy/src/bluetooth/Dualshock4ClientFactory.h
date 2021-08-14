
/**
 * @file Dualshock4ClientFactory.h
 */

/**********************************************************************

  Created: 25 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSHOCK4CLIENTFACTORY_H
#define _DUALSHOCK4CLIENTFACTORY_H

#include <array>

#include <src/utils/Logger.h>
#include <src/bluetooth/BluetoothAcceptor.h>

namespace dsremap
{
  /**
   * Acceptor client factory for the Dualshock4. This is instantiated
   * by SDPClientFactory when a connection is made on PSM 0x01. Its
   * role is to send the PDU response when appropriate and wait for
   * connections from the same bdaddr on PSMs 0x11 and 0x13. Then it
   * creates a new Dualshock4Proxy instance to handle the rest.
   */
  class Dualshock4ClientFactory : public BluetoothAcceptor::ClientFactory,
                                  public Application::Component,
                                  public Logger
  {
  public:
    Dualshock4ClientFactory(BluetoothAcceptor&, const std::string& addr, int fd);
    ~Dualshock4ClientFactory();

    bool on_new_connection(BluetoothAcceptor& acceptor, const std::string& addr, uint16_t psm, int fd) override;

    void stop() override;
    void reconfigure() override;

  private:
    BluetoothAcceptor& _acceptor;

    enum class State {
      SDP,
      WaitingConnect,
      Done
    };
    State _state;

    std::string _addr;
    int _fd_0x01;
    int _fd_0x11;
    int _fd_0x13;

    static gboolean static_io_callback(gint, GIOCondition, gpointer);
    bool io_callback(int, GIOCondition);
  };
}

#endif /* _DUALSHOCK4CLIENTFACTORY_H */
