
/**
 * @file HIDInterface.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _HIDINTERFACE_H
#define _HIDINTERFACE_H

#include <vector>
#include <cstdint>
#ifdef ENABLE_TIMING
#include <chrono>
#endif

#include <src/utils/Application.h>
#include <src/usb/Interface.h>

namespace dsremap
{
  class ControlEndpoint;

  class HIDInterface : public Interface
  {
  public:
    class Listener : public Application::ErrorHandler
    {
    public:
      virtual void on_usb_get_report(ControlEndpoint&, int type, int id) = 0;
      virtual void on_usb_set_report(int type, int id, const std::vector<uint8_t>& data) = 0;
      virtual void on_usb_out_report(int id, const std::vector<uint8_t>& data) = 0;
    };

    enum class Protocol {
      Boot,
      Report
    };

    HIDInterface(unsigned int index, Listener&);
    virtual ~HIDInterface();

    void enable_interrupt_in();
    void send_input_report(const uint8_t*, uint16_t);

    void add_in_endpoint(InEndpoint* ep) override;
    void add_out_endpoint(OutEndpoint* ep) override;

    void on_endpoint_data(OutEndpoint&, const std::vector<uint8_t>&) override;
    void on_error(std::exception_ptr) override;

    virtual const std::vector<uint8_t>& get_report_descriptor() const = 0;

    void parse_usb_descriptor(const std::vector<uint8_t>&) override;
    void handle_standard_request(ControlEndpoint&, const struct usb_ctrlrequest&) override;
    void handle_class_request(ControlEndpoint&, const struct usb_ctrlrequest&) override;

    virtual Protocol get_protocol() const {
      return _protocol;
    }

  protected:
    virtual void set_protocol(Protocol proto) {
      _protocol = proto;
    }

    Listener& listener() const {
      return _listener;
    }

  private:
    Listener& _listener;
    Protocol _protocol;
    bool _data_enabled;

    InEndpoint* _ep1;

#ifdef ENABLE_TIMING
    std::chrono::high_resolution_clock::time_point _last_timing;
    uint32_t _timing_count;
#endif
  };
}

#endif /* _HIDINTERFACE_H */
