/**
 * @file OutEndpoint.cpp
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <system_error>
#include <functional>

#include <glib-unix.h>

#include "USBDevice.h"
#include "OutEndpoint.h"
#include "Interface.h"

namespace dsremap
{
  OutEndpoint::OutEndpoint(USBDevice& dev, Interface* intf, unsigned int index, int fd, size_t max_packet_size)
    : Endpoint(dev, intf, index, fd),
      _max_packet_size(max_packet_size),
      _state(State::Disabled),
      _mtx(),
      _cond(),
      _thr(std::bind(&OutEndpoint::_thread_func, this))
  {
  }

  OutEndpoint::~OutEndpoint()
  {
    // Of course this assumes USBDevice called shutdown() first, and waited for on_endpoint_shutdown
    _thr.join();
  }

  void OutEndpoint::set_enabled(bool enabled)
  {
    {
      std::lock_guard<std::mutex> lock(_mtx);

      if (enabled && (_state == State::Disabled)) {
        _state = State::Enabled;
      } else if (!enabled && (_state == State::Enabled)) {
        _state = State::Disabled;
      }
    }

    _cond.notify_one();
  }

  void OutEndpoint::shutdown()
  {
    std::unique_lock<std::mutex> lock(_mtx);

    switch (_state) {
      case State::Enabled:
        pthread_kill(_thr.native_handle(), SIGINT);
        // No break
      case State::Disabled:
        _state = State::Stopping;
        lock.unlock();
        _cond.notify_one();
        // No break
      case State::Stopping:
        break;
    }
  }

  void OutEndpoint::_thread_func()
  {
    while (true) {
      std::unique_lock<std::mutex> lock(_mtx);
      _cond.wait(lock, [&]() { return _state != State::Disabled; });

      if (_state == State::Stopping)
        break;

      // Can only be Enabled now.

      lock.unlock();

      std::vector<uint8_t> data(_max_packet_size);
      ssize_t len = ::read(fd(), data.data(), _max_packet_size);
      if (len < 0) {
        _state = State::Disabled;

        if (errno == EINTR)
          break;

        try {
          throw std::system_error(errno, std::generic_category(), "Read error on OUT endpoint");
        } catch (...) {
          ErrorReport report(*this);
          report.wait();
          break;
        }
      } else if (len == 0) {
        try {
          throw std::runtime_error("OUT endpoint closed");
        } catch (...) {
          ErrorReport report(*this);
          report.wait();
          break;
        }
      } else {
        data.resize(len);
        DataReport report(*this, data);
        report.wait();
      }
    }

    // However the loop was exited, we need to wait for shutdown()
    std::unique_lock<std::mutex> lock(_mtx);
    _cond.wait(lock, [&]() { return _state == State::Stopping; });

    ShutdownReport report(*this, device());
    report.wait();
  }

  void OutEndpoint::ErrorReport::run()
  {
    ep.interface()->on_error(exc_ptr);
    done();
  }

  void OutEndpoint::DataReport::run()
  {
    try {
      ep.interface()->on_endpoint_data(ep, data);
    } catch (...) {
      try {
        ep.interface()->on_error(std::current_exception());
      } catch (...) {
        // Hum.
      }
    }
    done();
  }

  void OutEndpoint::ShutdownReport::run()
  {
    // Special case: the thread waits on the ShutdownReport so that it
    // is still valid when this is called from the main thread. But
    // on_endpoint_shutdown destroys the endpoint so we would
    // deadlock. So copy what we need on the stack, notify the thread
    // so it exits.
    USBDevice* dev = &_dev;
    OutEndpoint* endpoint = &ep;
    done();
    // Now the ShutdownReport may be invalid, but we have everything
    // we need on this stack
    dev->on_endpoint_shutdown(endpoint);
  }
}
