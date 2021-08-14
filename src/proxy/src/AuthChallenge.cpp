/**
 * @file AuthChallenge.cpp
 */

/**********************************************************************

  Created: 12 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <zlib.h>

#include <src/bluetooth/Proxy.h>
#include <src/usb/ControlEndpoint.h>

#include "AuthChallenge.h"

namespace dsremap
{
  AuthChallenge::AuthChallenge(Proxy& proxy)
    : Logger("Auth"),
      _state(State::Idle),
      _proxy(proxy),
      _current_seq(0x00),
      _current_rep(0),
      _responses()
  {
  }

  void AuthChallenge::set_report(int id, const std::vector<uint8_t>& data)
  {
    // We only get called for report 0xf0 anyway

    switch (_state) {
      case State::Idle:
      {
        debug("Challenge data available; seq={}; repcount={}", data[1], data[2]);

        // Forward to the Dualshock
        std::vector<uint8_t> report(65);
        report[0] = 0x53;
        memcpy(report.data() + 1, data.data(), data.size());
        _proxy.write_int_data(report);

        if (data[2] == 4) {
          debug("Challenge data set; moving to Waiting state");
          _current_seq = report[2];
          _state = State::Waiting;
        }

        break;
      }
      case State::Waiting:
      case State::Collecting:
      case State::Ready:
        warn("Unexpected SET_REPORT 0x{:02x} in state {}", id, static_cast<int>(_state));
        break;
    }
  }

  void AuthChallenge::get_report(ControlEndpoint& ep, int id)
  {
    switch (id) {
      case 0xf2:
        switch (_state) {
          case State::Idle:
            warn("Unexpected GET_REPORT 0xf2 in Idle state");
            break;
          case State::Waiting:
            debug("Get ready state from controller");
            _proxy.get_report(0xf2, 16);
            // No break
          case State::Collecting:
          {
            uint8_t report[16];
            memset(report, 0, sizeof(report));
            report[0] = 0xf2;
            report[1] = _current_seq;
            report[2] = 0x10; // Not ready

            uint32_t crc = crc32(0x00000000, report, sizeof(report) - 4);
            *((uint32_t*)(report + sizeof(report) - 4)) = crc;

            ep.write(report, sizeof(report));
            break;
          }
          case State::Ready:
          {
            uint8_t report[16];
            memset(report, 0, sizeof(report));
            report[0] = 0xf2;
            report[1] = _current_seq;
            report[2] = 0x00; // Ready

            uint32_t crc = crc32(0x00000000, report, sizeof(report) - 4);
            *((uint32_t*)(report + sizeof(report) - 4)) = crc;

            ep.write(report, sizeof(report));
            break;
          }
        }
        break;
      case 0xf1:
        debug("Sending challenge response for rep {}", _current_rep);
        ep.write(_responses[_current_rep++].data(), 64);
        if (_current_rep == 19) {
          debug("Sent all challenge response data; moving to state Idle");
          _state = State::Idle;
        }
        break;
      default:
        warn("Unexpected GET_REPORT 0x{:02x} in state {}", id, static_cast<int>(_state));
        break;
    }
  }

  void AuthChallenge::on_bt_data(const std::vector<uint8_t>& data)
  {
    switch (data[1]) {
      case 0xf1:
        switch (_state) {
          case State::Collecting:
          {
            debug("Got challenge response (rep={})", data[3]);
            memcpy(_responses[data[3]].data(), data.data() + 1, 60);
            uint32_t crc = crc32(0x00000000, _responses[data[3]].data(), 60);
            *((uint32_t*)(_responses[data[3]].data() + 60)) = crc;
            if (data[3] == 18) {
              debug("Challenge responses collected; moving to Ready state");
              _current_rep = 0;
              _state = State::Ready;
            }
            break;
          }
          case State::Idle:
          case State::Waiting:
          case State::Ready:
            warn("Unexpected 0xf1 report in state {}", static_cast<int>(_state));
            break;
        }
        break;
      case 0xf2:
        switch (_state ) {
          case State::Idle:
          case State::Collecting:
          case State::Ready:
            warn("Unexpected 0xf2 report in state {}", static_cast<int>(_state));
            break;
          case State::Waiting:
            debug("Ready state; seq={}", data[2]);
            debug_hex(data.data(), data.size());
            if (((data[3] >> 4) & 1) == 0) {
              debug("Dualshock ready; moving to Collecting state");
              _state = State::Collecting;
              for (uint8_t rep = 0; rep < 19; ++rep)
                _proxy.get_report(0xf1, 64);
            }
            break;
        }
        break;
      default:
        warn("Unexpected data in state {}", static_cast<int>(_state));
        debug_hex(data.data(), data.size());
        break;
    }
  }
}
