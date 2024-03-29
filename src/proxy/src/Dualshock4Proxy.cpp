/**
 * @file Dualshock4Proxy.cpp
 */

/**********************************************************************

  Created: 27 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

//#include <fstream>
#include <stdexcept>
#include <system_error>

#include <unistd.h>
#include <assert.h>

#include <glib-unix.h>

#include <src/utils/format.h>
#include <src/bluetooth/BTUtils.h>
#include <src/usb/Dualshock4HIDInterface.h>

#include "Dualshock4Proxy.h"

namespace dsremap
{
  Dualshock4Proxy::Dualshock4Proxy(BluetoothAcceptor& acceptor, int fd_0x11, int fd_0x13)
    : SonyControllerProxy(acceptor, fd_0x11, fd_0x13),
      _acceptor(acceptor),
      _report_06(),
      _report_a3(),
      _report_02_usb(),
      _report_02_bt(),
      _host_addr(),
      _usb_device(nullptr),
      _auth(*this)
  {
    std::vector<uint8_t> out_report(79);

    out_report[0x00] = 0xa2;
    out_report[0x01] = 0x11;
    out_report[0x02] = 0xc3; // 3ms poll interval
    out_report[0x03] = 0x20;
    out_report[0x04] = 0xf3;
    out_report[0x05] = 0x44;
    out_report[0x09] = 0xf6;
    out_report[0x0a] = 0xae;
    out_report[0x0b] = 0x99;
    out_report[0x16] = 0x31;
    out_report[0x17] = 0x31;
    out_report[0x19] = 0x4d;
    out_report[0x1a] = 0x85;

    debug("Set poll interval");
    write_out_data(out_report);

    debug("Get report 0x06");
    get_report(0x06, 0x0035);
  }

  //==============================================================================
  // Data from the Dualshock (BT)

  void Dualshock4Proxy::got_control_data(const std::vector<uint8_t>& data)
  {
    if (data.size() == 1) {
      switch (data[0]) {
        case 0x00:
          debug("ACK");
          break;
        case 0x0e:
          error("NACK: invalid CRC");
          break;
        default:
          error("NACK: 0x{:02x}", data[0]);
          break;
      }

      return;
    }

    // Feature report from the Dualshock (Bluetooth)
    switch (_state) {
      case State::GatheringReports:
        switch ((data[0] & 0xF0) >> 4) {
          case 0x0a: // DATA
            switch (data[1]) {
              case 0x06:
                debug("Got report 0x06");
                memcpy(_report_06.data(), data.data(), data.size());
                memcpy(_report_a3.data(), data.data(), data.size() - 4);
                _report_a3[1] = 0xa3;

                get_report(0x02,37);
                break;
              case 0x02:
              {
                debug("Got report 0x02");
                debug_hex(data.data(), data.size());

                memcpy(_report_02_bt.data(), data.data(), data.size());
                memcpy(_report_02_usb.data(), data.data() + 1, data.size() - 1);

                //                                 gx-   gy+   gy-   gz+
                // USB: f7 ff 0a 00 ef ff b9 22 - 33 dd d2 22 43 dd b4 22 - 27 dd 1c 02 1c 02 a9 1f c4 e0 9d 20 70 e0 a7 20 7b df 04 00
                // BT:  f7 ff 0a 00 ef ff b9 22 - d2 22 b4 22 33 dd 43 dd - 27 dd 1c 02 1c 02 a9 1f c4 e0 9d 20 70 e0 a7 20 7b df 04 00

                CalibrationData_t* calib = (CalibrationData_t*)(_report_02_usb.data() + 1);
                calib->gyro_x_minus = ((CalibrationData_t*)(data.data() + 2))->gyro_y_minus;
                calib->gyro_y_plus = ((CalibrationData_t*)(data.data() + 2))->gyro_x_minus;
                calib->gyro_y_minus = ((CalibrationData_t*)(data.data() + 2))->gyro_z_plus;
                calib->gyro_z_plus = ((CalibrationData_t*)(data.data() + 2))->gyro_y_plus;

                _imu.SetCalibrationData(calib);

                // That's all we need, attach USB now
                debug("Attaching USB device");
                _state = State::Attaching;
                _usb_device.reset(new Dualshock4(*this, *this));
                _usb_device->attach();
                break;
              }
              default:
                warn("Got unsollicited report 0x{:02x} in state GatheringReports", data[1]);
                break;
            }
            break;
          default:
            warn("Ignoring control data from Dualshock (BT)");
            debug_hex(data.data(), data.size());
            break;
        }
        break;
      case State::Attaching:
      case State::SendPS:
      case State::Closing:
      case State::Connecting:
        break;
      case State::Running:
        switch (data[1]) {
          case 0xf1:
          case 0xf2:
            _auth.on_bt_data(data);
            break;
          default:
            warn("Ignoring control data from Dualshock (BT)");
            debug_hex(data.data(), data.size());
            break;
        }
        break;
    }
  }

  void Dualshock4Proxy::got_interrupt_data(const std::vector<uint8_t>& data)
  {
    // IN report from the Dualshock (Bluetooth)

    switch (_state) {
      case State::GatheringReports:
      case State::Attaching:
        break;
      case State::SendPS:
      case State::Connecting:
      case State::Running:
      {
        // Those are the main IN reports; we transfer them to the PS4
        // through the USB gadget
        if ((data[0] == 0xa1) && (data[1] == 0x11)) {
          Dualshock4HIDInterface::InputReport report(data);

          imu_state_t imu;
          report.get_imu(&imu);
          _imu.Update(&imu);

          controller_state_t ctrl;
          report.get_ctrl(&ctrl);

          if (_vm.get())
            _vm->Run(&ctrl, &_imu);

          if (_state == State::SendPS)
            ctrl.PS = ((_pscount++ % 50) == 0) ? 0 : 1;

          report.set_ctrl(&ctrl);
          report.send(_usb_device->hid_interface());
        }

        break;
      }
      case State::Closing:
        break;
    }
  }

  //==============================================================================
  // Data from the PS4 (USB)

  void Dualshock4Proxy::on_usb_get_report(ControlEndpoint& ep, int type, int id)
  {
    switch (type) {
      case 0x03:
        switch (id) {
          case 0xa3:
            debug("Send report 0xa3");
            ep.write(_report_a3.data() + 1, _report_a3.size() - 1);
            break;
          case 0x02:
            debug("Send report 0x02");
            ep.write(_report_02_usb.data(), _report_02_usb.size());
            break;
          case 0x12: // Get pairing state
          {
            std::array<uint8_t, 16> report;
            report[0x00] = 0x12;

            HCIDevice(0).get_bdaddr((bdaddr_t*)(report.data() + 1));

            report[0x07] = 0x08;
            report[0x08] = 0x25;
            report[0x09] = 0x00;

            memset(report.data() + 0x0a, 0, 6); // The PS4 address

            debug("Send report 0x12");
            ep.write(report.data(), report.size());
            break;
          }
          case 0xf1:
          case 0xf2:
            _auth.get_report(ep, id);
            break;
          default:
            error("Unhandled GET_REPORT type=0x{:02x} id=0x{:02x}", type, id);
            break;
        }
        break;
      default:
        error("Unhandled GET_REPORT type=0x{:02x} id=0x{:02x}", type, id);
        break;
    }
  }

  void Dualshock4Proxy::on_usb_set_report(int type, int id, const std::vector<uint8_t>& data)
  {
    switch (type) {
      case 0x03:
        switch (id) {
          case 0x13:
          {
            debug("Got SET_REPORT 0x13");
            memcpy(_host_addr.data(), data.data() + 1, 6);
            debug("Link key:");
            debug_hex(data.data() + 7, 16, true);
            HCIDevice(0).set_link_key((bdaddr_t*)_host_addr.data(), data.data() + 7);

            break;
          }
          case 0x14:
            switch (data[1]) {
              case 0x02:
                debug("Ignoring unpair command");
                _pscount = 0;
                _state = State::SendPS;
                _usb_device->hid_interface().enable_interrupt_in();
                break;
              case 0x01:
                debug("Pair command; connecting to the PS4");
                _state = State::Connecting;
                _bt_device.reset(new BTDevice(_acceptor, *this, (bdaddr_t*)_host_addr.data()));
                break;
              default:
                warn("Ignoring unknown pairing command 0x{:02x}", data[1]);
                break;
            }
            break;
          case 0xf0:
            _auth.set_report(id, data);
            break;
          default:
            error("Unhandled SET_REPORT type=0x{:02x} id=0x{:02x}", type, id);
            debug_hex(data.data(), data.size());
            break;
        }
        break;
      default:
        error("Unhandled SET_REPORT type=0x{:02x} id=0x{:02x}", type, id);
        debug_hex(data.data(), data.size());
        break;
    }
  }

  void Dualshock4Proxy::on_usb_out_report(int id, const std::vector<uint8_t>& data)
  {
    if (id == 0x05) {
      std::vector<uint8_t> out_report(79);

      out_report[0x00] = 0xa2;
      out_report[0x01] = 0x11;
      out_report[0x02] = 0xc3; // 3ms poll interval
      out_report[0x03] = 0x20;
      out_report[0x04] = 0xf3;
      out_report[0x05] = 0x44;
      memcpy(out_report.data() + 0x07, data.data() + 4, data.size() - 4);
      out_report[0x16] = 0x31;
      out_report[0x17] = 0x31;
      out_report[0x19] = 0x4d;
      out_report[0x1a] = 0x85;

      try {
        write_out_data(out_report);
      } catch (const std::exception& exc) {
        error("Write error on BTHID interrupt channel: {}", exc.what());
        stop();
      }
    }
  }

  //==============================================================================
  // Data from the PS4 (BT)

  void Dualshock4Proxy::on_bt_connected()
  {
    debug("Connected; moving to state Running");
    _state = State::Running;
  }

  void Dualshock4Proxy::on_bt_get_report(int fd, int type, int id)
  {
    switch (id) {
      case 0x06:
        debug("Sending report 0x06");
        if (::write(fd, _report_06.data(), _report_06.size()) < 0)
          throw std::system_error(errno, std::generic_category(), "Cannot write report 0x06");
        break;
      case 0x02:
        debug("Sending report 0x02");
        if (::write(fd, _report_02_bt.data(), _report_02_bt.size()) < 0)
          throw std::system_error(errno, std::generic_category(), "Cannot write report 0x02");
        break;
      case 0xa3:
        debug("Sending report 0xa3");
        if (::write(fd, _report_a3.data(), _report_a3.size()) < 0)
          throw std::system_error(errno, std::generic_category(), "Cannot write report 0xa3");
        break;
      default:
        throw std::runtime_error(format("Unsupported report 0x{:02x} requested", id));
    }
  }

  void Dualshock4Proxy::on_bt_set_report(int type, int id, const std::vector<uint8_t>& data)
  {
    warn("Ignoring SET_REPORT (BT) from PS4");
    debug_hex(data.data(), data.size());
  }

  void Dualshock4Proxy::on_bt_out_report(int id, const std::vector<uint8_t>& data)
  {
    // Don't care for now
  }

  const std::vector<uint8_t> Dualshock4Proxy::_ssa_response = {
    0x07 ,0x00 ,0x01 ,0x02 ,0xBF ,0x02 ,0xBC ,0x36 ,0x02 ,0xB9 ,0x36 ,0x02 ,0x61 ,0x09 ,0x00 ,0x00,
    0x0A ,0x00 ,0x01 ,0x00 ,0x01 ,0x09 ,0x00 ,0x01 ,0x35 ,0x03 ,0x19 ,0x11 ,0x24 ,0x09 ,0x00 ,0x04,
    0x35 ,0x0D ,0x35 ,0x06 ,0x19 ,0x01 ,0x00 ,0x09 ,0x00 ,0x11 ,0x35 ,0x03 ,0x19 ,0x00 ,0x11 ,0x09,
    0x00 ,0x06 ,0x35 ,0x09 ,0x09 ,0x65 ,0x6E ,0x09 ,0x00 ,0x6A ,0x09 ,0x01 ,0x00 ,0x09 ,0x00 ,0x09,
    0x35 ,0x08 ,0x35 ,0x06 ,0x19 ,0x11 ,0x24 ,0x09 ,0x01 ,0x00 ,0x09 ,0x00 ,0x0D ,0x35 ,0x0F ,0x35,
    0x0D ,0x35 ,0x06 ,0x19 ,0x01 ,0x00 ,0x09 ,0x00 ,0x13 ,0x35 ,0x03 ,0x19 ,0x00 ,0x11 ,0x09 ,0x01,
    0x00 ,0x25 ,0x13 ,0x57 ,0x69 ,0x72 ,0x65 ,0x6C ,0x65 ,0x73 ,0x73 ,0x20 ,0x43 ,0x6F ,0x6E ,0x74,
    0x72 ,0x6F ,0x6C ,0x6C ,0x65 ,0x72 ,0x09 ,0x01 ,0x01 ,0x25 ,0x0F ,0x47 ,0x61 ,0x6D ,0x65 ,0x20,
    0x43 ,0x6F ,0x6E ,0x74 ,0x72 ,0x6F ,0x6C ,0x6C ,0x65 ,0x72 ,0x09 ,0x01 ,0x02 ,0x25 ,0x1B ,0x53,
    0x6F ,0x6E ,0x79 ,0x20 ,0x43 ,0x6F ,0x6D ,0x70 ,0x75 ,0x74 ,0x65 ,0x72 ,0x20 ,0x45 ,0x6E ,0x74,
    0x65 ,0x72 ,0x74 ,0x61 ,0x69 ,0x6E ,0x6D ,0x65 ,0x6E ,0x74 ,0x09 ,0x02 ,0x00 ,0x09 ,0x01 ,0x00,
    0x09 ,0x02 ,0x01 ,0x09 ,0x01 ,0x11 ,0x09 ,0x02 ,0x02 ,0x08 ,0x08 ,0x09 ,0x02 ,0x03 ,0x08 ,0x00,
    0x09 ,0x02 ,0x04 ,0x28 ,0x00 ,0x09 ,0x02 ,0x05 ,0x28 ,0x01 ,0x09 ,0x02 ,0x06 ,0x36 ,0x01 ,0x6C,
    0x36 ,0x01 ,0x69 ,0x08 ,0x22 ,0x26 ,0x01 ,0x64 ,0x05 ,0x01 ,0x09 ,0x05 ,0xA1 ,0x01 ,0x85 ,0x01,
    0x09 ,0x30 ,0x09 ,0x31 ,0x09 ,0x32 ,0x09 ,0x35 ,0x15 ,0x00 ,0x26 ,0xFF ,0x00 ,0x75 ,0x08 ,0x95,
    0x04 ,0x81 ,0x02 ,0x09 ,0x39 ,0x15 ,0x00 ,0x25 ,0x07 ,0x75 ,0x04 ,0x95 ,0x01 ,0x81 ,0x42 ,0x05,
    0x09 ,0x19 ,0x01 ,0x29 ,0x0E ,0x15 ,0x00 ,0x25 ,0x01 ,0x75 ,0x01 ,0x95 ,0x0E ,0x81 ,0x02 ,0x75,
    0x06 ,0x95 ,0x01 ,0x81 ,0x01 ,0x05 ,0x01 ,0x09 ,0x33 ,0x09 ,0x34 ,0x15 ,0x00 ,0x26 ,0xFF ,0x00,
    0x75 ,0x08 ,0x95 ,0x02 ,0x81 ,0x02 ,0x06 ,0x04 ,0xFF ,0x85 ,0x02 ,0x09 ,0x24 ,0x95 ,0x24 ,0xB1,
    0x02 ,0x85 ,0xA3 ,0x09 ,0x25 ,0x95 ,0x30 ,0xB1 ,0x02 ,0x85 ,0x05 ,0x09 ,0x26 ,0x95 ,0x28 ,0xB1,
    0x02 ,0x85 ,0x06 ,0x09 ,0x27 ,0x95 ,0x34 ,0xB1 ,0x02 ,0x85 ,0x07 ,0x09 ,0x28 ,0x95 ,0x30 ,0xB1,
    0x02 ,0x85 ,0x08 ,0x09 ,0x29 ,0x95 ,0x2F ,0xB1 ,0x02 ,0x06 ,0x03 ,0xFF ,0x85 ,0x03 ,0x09 ,0x21,
    0x95 ,0x26 ,0xB1 ,0x02 ,0x85 ,0x04 ,0x09 ,0x22 ,0x95 ,0x2E ,0xB1 ,0x02 ,0x85 ,0xF0 ,0x09 ,0x47,
    0x95 ,0x3F ,0xB1 ,0x02 ,0x85 ,0xF1 ,0x09 ,0x48 ,0x95 ,0x3F ,0xB1 ,0x02 ,0x85 ,0xF2 ,0x09 ,0x49,
    0x95 ,0x0F ,0xB1 ,0x02 ,0x06 ,0x00 ,0xFF ,0x85 ,0x11 ,0x09 ,0x20 ,0x15 ,0x00 ,0x26 ,0xFF ,0x00,
    0x75 ,0x08 ,0x95 ,0x4D ,0x81 ,0x02 ,0x09 ,0x21 ,0x91 ,0x02 ,0x85 ,0x12 ,0x09 ,0x22 ,0x95 ,0x8D,
    0x81 ,0x02 ,0x09 ,0x23 ,0x91 ,0x02 ,0x85 ,0x13 ,0x09 ,0x24 ,0x95 ,0xCD ,0x81 ,0x02 ,0x09 ,0x25,
    0x91 ,0x02 ,0x85 ,0x14 ,0x09 ,0x26 ,0x96 ,0x0D ,0x01 ,0x81 ,0x02 ,0x09 ,0x27 ,0x91 ,0x02 ,0x85,
    0x15 ,0x09 ,0x28 ,0x96 ,0x4D ,0x01 ,0x81 ,0x02 ,0x09 ,0x29 ,0x91 ,0x02 ,0x85 ,0x16 ,0x09 ,0x2A,
    0x96 ,0x8D ,0x01 ,0x81 ,0x02 ,0x09 ,0x2B ,0x91 ,0x02 ,0x85 ,0x17 ,0x09 ,0x2C ,0x96 ,0xCD ,0x01,
    0x81 ,0x02 ,0x09 ,0x2D ,0x91 ,0x02 ,0x85 ,0x18 ,0x09 ,0x2E ,0x96 ,0x0D ,0x02 ,0x81 ,0x02 ,0x09,
    0x2F ,0x91 ,0x02 ,0x85 ,0x19 ,0x09 ,0x30 ,0x96 ,0x22 ,0x02 ,0x81 ,0x02 ,0x09 ,0x31 ,0x91 ,0x02,
    0x06 ,0x80 ,0xFF ,0x85 ,0x82 ,0x09 ,0x22 ,0x95 ,0x3F ,0xB1 ,0x02 ,0x85 ,0x83 ,0x09 ,0x23 ,0xB1,
    0x02 ,0x85 ,0x84 ,0x09 ,0x24 ,0xB1 ,0x02 ,0x85 ,0x90 ,0x09 ,0x30 ,0xB1 ,0x02 ,0x85 ,0x91 ,0x09,
    0x31 ,0xB1 ,0x02 ,0x85 ,0x92 ,0x09 ,0x32 ,0xB1 ,0x02 ,0x85 ,0x93 ,0x09 ,0x33 ,0xB1 ,0x02 ,0x85,
    0xA0 ,0x09 ,0x40 ,0xB1 ,0x02 ,0x85 ,0xA4 ,0x09 ,0x44 ,0xB1 ,0x02 ,0xC0 ,0x09 ,0x02 ,0x07 ,0x35,
    0x08 ,0x35 ,0x06 ,0x09 ,0x04 ,0x09 ,0x09 ,0x01 ,0x00 ,0x09 ,0x02 ,0x08 ,0x28 ,0x00 ,0x09 ,0x02,
    0x09 ,0x28 ,0x01 ,0x09 ,0x02 ,0x0A ,0x28 ,0x01 ,0x09 ,0x02 ,0x0B ,0x09 ,0x01 ,0x00 ,0x09 ,0x02,
    0x0C ,0x09 ,0x1F ,0x40 ,0x09 ,0x02 ,0x0D ,0x28 ,0x00 ,0x09 ,0x02 ,0x0E ,0x28 ,0x00 ,0x36 ,0x00,
    0x52 ,0x09 ,0x00 ,0x00 ,0x0A ,0x00 ,0x01 ,0x00 ,0x02 ,0x09 ,0x00 ,0x01 ,0x35 ,0x03 ,0x19 ,0x12,
    0x00 ,0x09 ,0x00 ,0x04 ,0x35 ,0x0D ,0x35 ,0x06 ,0x19 ,0x01 ,0x00 ,0x09 ,0x00 ,0x01 ,0x35 ,0x03,
    0x19 ,0x00 ,0x01 ,0x09 ,0x00 ,0x09 ,0x35 ,0x08 ,0x35 ,0x06 ,0x19 ,0x12 ,0x00 ,0x09 ,0x01 ,0x03,
    0x09 ,0x02 ,0x00 ,0x09 ,0x01 ,0x03 ,0x09 ,0x02 ,0x01 ,0x09 ,0x05 ,0x4C ,0x09 ,0x02 ,0x02 ,0x09,
    0x05 ,0xC4 ,0x09 ,0x02 ,0x03 ,0x09 ,0x01 ,0x00 ,0x09 ,0x02 ,0x04 ,0x28 ,0x01 ,0x09 ,0x02 ,0x05,
    0x09 ,0x00 ,0x02 ,0x00
    };
}
