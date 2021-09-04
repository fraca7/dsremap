
/**
 * @file Dualshock4HIDInterface.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSHOCK4HIDINTERFACE_H
#define _DUALSHOCK4HIDINTERFACE_H

#include <vector>

#include <src/usb/HIDInterface.h>

namespace dsremap
{
  class Dualshock4HIDInterface : public HIDInterface
  {
  public:
    class InputReport : public HIDInterface::InputReport
    {
    public:
      InputReport(const std::vector<uint8_t>& bluetooth_data);

      void get_imu(imu_state_t*) const override;
      void get_ctrl(controller_state_t*) const override;
      void set_ctrl(const controller_state_t*) override;

      void send(HIDInterface&) const override;

    private:
      USBReport01_t _report;
    };

    Dualshock4HIDInterface(HIDInterface::Listener&);
    ~Dualshock4HIDInterface();

    const std::vector<uint8_t>& get_report_descriptor() const override;

  private:
    static std::vector<uint8_t> _report_descriptor;
  };
}

#endif /* _DUALSHOCK4HIDINTERFACE_H */
