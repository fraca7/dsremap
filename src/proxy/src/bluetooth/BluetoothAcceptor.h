
/**
 * @file BluetoothAcceptor.h
 */

/**********************************************************************

  Created: 25 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _BLUETOOTHACCEPTOR_H
#define _BLUETOOTHACCEPTOR_H

#include <list>

#include <glib.h>

#include <src/utils/Logger.h>
#include <src/utils/Application.h>

namespace dsremap
{
  class SDPClientFactory;

  class BluetoothAcceptor : public Logger,
                            public Application
  {
  public:
    /**
     * New connections go through a chain of responsibility. Each link
     * should subclass this and register itself using add_listener.
     */
    class ClientFactory {
    public:
      /**
       * Return true if the new connection was handled, to stop propagation.
       */
      virtual bool on_new_connection(BluetoothAcceptor& acceptor, const std::string& addr, uint16_t psm, int fd) = 0;
    };

    BluetoothAcceptor();
    ~BluetoothAcceptor();

    /**
     * Add a new link to the chain of responsibility. Note that links
     * will be called in reverse order, i.e. last added first.
     */
    void add_client_factory(ClientFactory*);

    /**
     * Remove a link.
     */
    void remove_client_factory(ClientFactory*);

  private:
    // Listen on 3 PSMs: SDP, HID control, HID interrupt
    struct Descriptor {
      int fd;
      uint16_t psm;
    };
    std::list<Descriptor> _descriptors;

    static gboolean static_accept_callback(gint, GIOCondition, gpointer);
    bool accept_callback(int);

    // The factory is actually the first (last) link in the chain
    SDPClientFactory* _factory;
    std::list<ClientFactory*> _client_factories;
  };
}

#endif /* _BLUETOOTHACCEPTOR_H */
