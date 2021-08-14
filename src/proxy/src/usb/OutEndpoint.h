
/**
 * @file OutEndpoint.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _OUTENDPOINT_H
#define _OUTENDPOINT_H

#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <src/utils/Application.h>
#include <src/usb/Endpoint.h>

namespace dsremap
{
  class OutEndpoint : public Endpoint
  {
  public:
    class Listener : public Application::ErrorHandler
    {
    public:
      virtual void on_endpoint_data(OutEndpoint&, const std::vector<uint8_t>&) = 0;
    };

    OutEndpoint(USBDevice& dev, Interface* intf, unsigned int index, int fd, size_t max_packet_size);
    virtual ~OutEndpoint();

    void set_enabled(bool);
    void shutdown() override;

  private:
    size_t _max_packet_size;
    enum class State {
      Disabled,
      Enabled,
      Stopping
    };
    State _state;

    std::mutex _mtx;
    std::condition_variable _cond;
    std::thread _thr;

    void _thread_func();

    class Report {
    public:
      Report(OutEndpoint& ep)
        : ep(ep),
          _mtx(),
          _cond(),
          _done(false) {
        g_idle_add_full(G_PRIORITY_HIGH, &Report::static_callback, static_cast<gpointer>(this), NULL);
      }

      virtual void run() = 0;

      void wait() {
        std::unique_lock<std::mutex> lock(_mtx);
        _cond.wait(lock, [&]() { return _done; });
      }

    protected:
      OutEndpoint& ep;

      void done() {
        {
          std::lock_guard<std::mutex> lock(_mtx);
          _done = true;
        }
        _cond.notify_one();
      }

    private:
      std::mutex _mtx;
      std::condition_variable _cond;
      bool _done;

      static gboolean static_callback(gpointer ptr) {
        Report* self = static_cast<Report*>(ptr);

        try {
          self->run();
        } catch (...) {
          self->done();
        }

        return false;
      }
    };

    class ErrorReport : public Report {
    public:
      ErrorReport(OutEndpoint& ep)
        : Report(ep),
          exc_ptr(std::current_exception()) {
      }

      void run() override;

    private:
      std::exception_ptr exc_ptr;
    };

    class DataReport : public Report {
    public:
      DataReport(OutEndpoint& ep, const std::vector<uint8_t>& data)
        : Report(ep),
          data(data) {
      }

      void run() override;

    private:
      const std::vector<uint8_t>& data;
    };

    class ShutdownReport : public Report {
    public:
      ShutdownReport(OutEndpoint& ep, USBDevice& dev)
        : Report(ep),
          _dev(dev) {
      }

      void run() override;

    private:
      USBDevice& _dev;
    };
  };
}

#endif /* _OUTENDPOINT_H */
