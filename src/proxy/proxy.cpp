/**
 * @file proxy.cpp
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 Quividi

**********************************************************************/

#include <fstream>
#include <stdexcept>
#include <iostream>

#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <glib.h>
#include <glib-unix.h>

#include <src/utils/Logger.h>
#include <src/utils/File.h>
#include <src/bluetooth/BluetoothAcceptor.h>
#include <src/HTTPServer.h>
#include <src/Dualshock4Proxy.h>

using namespace std;
using namespace dsremap;

class Dualshock4App : public BluetoothAcceptor
{
public:
  using BluetoothAcceptor::BluetoothAcceptor;

  void create_proxy(int fd_0x11, int fd_0x13) {
    new Dualshock4Proxy(*this, fd_0x11, fd_0x13);
  }
};

int main(int argc, char* argv[])
{
  Logger logger("dsremap-proxy");

  if (geteuid() != 0) {
    cerr << "Must be run as root." << endl;
    return 1;
  }

  File pidfile("/var/run/dsremap-proxy.pid");

  {
    std::ofstream ofs(pidfile.as_string(), std::ios::trunc);
    ofs << getpid();
  }

  try {
    Dualshock4App app;
    HTTPServer srv(app, 8000, CONFIG_PATH);

    app.run();
  } catch (const exception& exc) {
    pidfile.unlink();
    cerr << exc.what() << endl;
    return 1;
  }

  pidfile.unlink();
  return 0;
}
