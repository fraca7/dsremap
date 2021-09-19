/**
 * @file BTDevice.cpp
 */

/**********************************************************************

  Created: 06 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <unistd.h>
#include <glib-unix.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <stdlib.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include <system_error>

#include <src/utils/format.h>

#include "BTUtils.h"
#include "BTDevice.h"

#define ACL_MTU 1024

static const uint8_t sdp_pdu_request[] = {
  0x06, 0x00, 0x01, 0x00, 0x0f, 0x35, 0x03, 0x19, 0x01, 0x00, 0x08, 0x00, 0x35, 0x05, 0x0a, 0x00, 0x00, 0xff, 0xff, 0x00
};

namespace dsremap
{
  BTDevice::BTDevice(BluetoothAcceptor& acceptor, Listener& listener, const bdaddr_t* addr)
    : Logger("BTClient"),
      _acceptor(acceptor),
      _listener(listener),
      _fds({ -1, -1, -1 }),
      _connected({ false, false, false })
  {
    acceptor.add_client_factory(this);

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

    _acceptor.remove_client_factory(this);
  }

  bool BTDevice::on_new_connection(BluetoothAcceptor&, const std::string& addr, uint16_t psm, uint16_t cid, int fd)
  {
    if (psm == 0x01) {
      // Lifted from https://github.com/matlo/l2cap_proxy

      bdaddr_t dst_addr;
      str2ba(addr.c_str(), &dst_addr);

      auto data = _listener.get_ssa_response();
      info("First connection to this console; sending SSA ({} bytes)", data.size());

      struct hci_conn_info_req* cr = (struct hci_conn_info_req*)malloc(sizeof(struct hci_conn_info_req) + sizeof(struct hci_conn_info));
      bacpy(&cr->bdaddr, &dst_addr);
      cr->type = ACL_LINK;
      try {
        int device;
        if ((device = hci_get_route(const_cast<bdaddr_t*>(&dst_addr))) < 0)
          throw std::system_error(errno, std::generic_category(), "hci_get_route");

        int dd;
        if ((dd = hci_open_dev(device)) < 0)
          throw std::system_error(errno, std::generic_category(), "hci_open_dev");

        try {
          if (ioctl(dd, HCIGETCONNINFO, (unsigned long)cr) < 0)
            throw std::system_error(errno, std::generic_category(), "ioctl HCIGETCONNINFO");
  
          uint16_t data_len = ACL_MTU - 1 - HCI_ACL_HDR_SIZE - L2CAP_HDR_SIZE;
          if (data.size() < data_len)
            data_len = data.size();

          struct iovec iv[4];
          uint8_t type = HCI_ACLDATA_PKT;

          iv[0].iov_base = &type;
          iv[0].iov_len = 1;

          hci_acl_hdr acl_hdr;
          acl_hdr.handle = htobs(acl_handle_pack(cr->conn_info->handle, ACL_START));
          acl_hdr.dlen = htobs(data_len + L2CAP_HDR_SIZE);
  
          iv[1].iov_base = &acl_hdr;
          iv[1].iov_len = HCI_ACL_HDR_SIZE;

          l2cap_hdr l2_hdr;
          l2_hdr.cid = htobs(cid);
          l2_hdr.len = htobs(data.size());

          iv[2].iov_base = &l2_hdr;
          iv[2].iov_len = L2CAP_HDR_SIZE;

          int ivn = 3;
  
          if (data_len)
          {
            iv[3].iov_base = const_cast<uint8_t*>(data.data());
            iv[3].iov_len = htobs(data_len);
            ivn = 4;
          }
  
          while (writev(dd, iv, ivn) < 0)
          {
            if (errno == EAGAIN || errno == EINTR)
              continue;
            throw std::system_error(errno, std::generic_category(), "writev");
          }

          size_t plen = data.size() - data_len;
          const uint8_t* pdata = data.data();

          while(plen)
          {
            pdata += data_len;
            data_len = ACL_MTU - 1 - HCI_ACL_HDR_SIZE;
            if(plen < data_len)
              data_len = plen;

            iv[0].iov_base = &type;
            iv[0].iov_len = 1;

            acl_hdr.handle = htobs(acl_handle_pack(cr->conn_info->handle, ACL_CONT));
            acl_hdr.dlen = htobs(plen);

            iv[1].iov_base = &acl_hdr;
            iv[1].iov_len = HCI_ACL_HDR_SIZE;

            iv[2].iov_base = const_cast<uint8_t*>(pdata);
            iv[2].iov_len = htobs(data_len);
            ivn = 3;
  
            while (writev(dd, iv, ivn) < 0)
            {
              if (errno == EAGAIN || errno == EINTR)
                continue;
              throw std::system_error(errno, std::generic_category(), "writev");
            }
    
            plen -= data_len;
          }

          free(cr);
          close(dd);
          close(fd);
        } catch (...) {
          close(dd);
          close(fd);
          throw;
        }
      } catch (...) {
        free(cr);
        close(fd);
        throw;
      }

      close(fd);
      return true;
    }

    return false;
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
