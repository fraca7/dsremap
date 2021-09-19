
/**
 * @file BTDevice.h
 */

/**********************************************************************

  Created: 06 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _BTDEVICE_H
#define _BTDEVICE_H

#include <stdexcept>
#include <vector>
#include <array>

#include <glib.h>
#include <bluetooth/bluetooth.h>

#include <src/bluetooth/BluetoothAcceptor.h>
#include <src/utils/Application.h>
#include <src/utils/Logger.h>

namespace dsremap
{
  /**
   * Connection to the host; behaves like a Bluetooth device
   */
  class BTDevice : public Logger, public BluetoothAcceptor::ClientFactory
  {
  public:
    class Listener : public Application::ErrorHandler {
    public:
      virtual void on_bt_connected() = 0;
      virtual void on_bt_get_report(int fd, int type, int id) = 0;
      virtual void on_bt_set_report(int type, int id, const std::vector<uint8_t>& data) = 0;
      virtual void on_bt_out_report(int id, const std::vector<uint8_t>&) = 0;

      virtual const std::vector<uint8_t>& get_ssa_response() = 0;
    };

    BTDevice(BluetoothAcceptor&, Listener&, const bdaddr_t*);
    ~BTDevice();

    bool on_new_connection(BluetoothAcceptor&, const std::string&, uint16_t, uint16_t, int) override;

  private:
    BluetoothAcceptor& _acceptor;
    Listener& _listener;
    bdaddr_t _host_addr;
    std::array<int, 3> _fds;
    std::array<bool, 3> _connected;

    static gboolean static_io_callback(int, GIOCondition, gpointer);
    bool io_callback(int, GIOCondition);

    void _connect(int index);
  };
}

#endif /* _BTDEVICE_H */
