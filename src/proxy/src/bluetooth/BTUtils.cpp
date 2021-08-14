/**
 * @file BTUtils.cpp
 */

/**********************************************************************

  Created: 06 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <system_error>

#include <unistd.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/l2cap.h>

#include <src/utils/format.h>

#include "BTUtils.h"

namespace dsremap
{
  void set_l2cap_socket_options(int fd)
  {
    int opt = L2CAP_LM_MASTER;
    if (::setsockopt(fd, SOL_L2CAP, L2CAP_LM, &opt, sizeof(opt)) < 0)
      throw std::system_error(errno, std::generic_category(), "Cannot set L2CAP_LM_MASTER");

    struct bt_power pwr = { .force_active = BT_POWER_FORCE_ACTIVE_OFF };
    if (::setsockopt(fd, SOL_BLUETOOTH, BT_POWER, &pwr, sizeof(pwr)) < 0)
      throw std::system_error(errno, std::generic_category(), "Cannot set BT_POWER");
  }

  HCIDevice::HCIDevice(unsigned int n)
  {
    _dd = hci_open_dev(n);
    if (_dd < 0)
      throw std::system_error(errno, std::generic_category(), format("Cannot open HCI device #{}", n));
  }

  HCIDevice::~HCIDevice()
  {
    ::close(_dd);
  }

  void HCIDevice::set_link_key(bdaddr_t* addr, const uint8_t* key)
  {
    // Hum.
    uint8_t wk[16];
    memcpy(wk, key, 16);
    if (hci_write_stored_link_key(_dd, addr, wk, 1000) < 0)
      throw std::system_error(errno, std::generic_category(), "Cannot write link key to device");
  }

  void HCIDevice::get_bdaddr(bdaddr_t* addr)
  {
    if (hci_read_bd_addr(_dd, addr, 1000) < 0)
      throw std::system_error(errno, std::generic_category(), "Cannot get device BDADDR");
  }
}
