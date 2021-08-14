
/**
 * @file BTUtils.h
 */

/**********************************************************************

  Created: 06 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _BTUTILS_H
#define _BTUTILS_H

#include <bluetooth/bluetooth.h>

namespace dsremap
{
  void set_l2cap_socket_options(int fd);

  class HCIDevice
  {
  public:
    HCIDevice(unsigned int);
    ~HCIDevice();

    void set_link_key(bdaddr_t*, const uint8_t*);
    void get_bdaddr(bdaddr_t*);

  private:
    int _dd;
  };
}

#endif /* _BTUTILS_H */
