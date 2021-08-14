
/**
 * @file Dualshock4HIDInterface.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSHOCK4HIDINTERFACE_H
#define _DUALSHOCK4HIDINTERFACE_H

#ifdef ENABLE_TIMING
#include <chrono>
#endif

#include <vector>
#include <array>

#include <dsremap/DS4Structs.h>
#include <src/usb/HIDInterface.h>

namespace dsremap
{
  class Dualshock4HIDInterface : public HIDInterface
  {
  public:
    Dualshock4HIDInterface(HIDInterface::Listener&);
    virtual ~Dualshock4HIDInterface();

    void enable_interrupt_in();

    void send_input_report(USBReport01_t&);

    void add_in_endpoint(InEndpoint* ep) override;
    void add_out_endpoint(OutEndpoint* ep) override;

    void on_endpoint_data(OutEndpoint&, const std::vector<uint8_t>&) override;
    void on_error(std::exception_ptr) override;

    const std::vector<uint8_t>& get_report_descriptor() const override;

  private:
    bool _data_enabled;

    InEndpoint* ep1;

#ifdef ENABLE_TIMING
    std::chrono::high_resolution_clock::time_point _last_timing;
    uint32_t _timing_count;
#endif

    static std::vector<uint8_t> _report_descriptor;
  };
}

#endif /* _DUALSHOCK4HIDINTERFACE_H */
