
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

#include <dsremap/DS4Structs.h>
#include <src/usb/HIDInterface.h>

namespace dsremap
{
  class Dualshock4HIDInterface : public HIDInterface
  {
  public:
    Dualshock4HIDInterface(HIDInterface::Listener&);
    ~Dualshock4HIDInterface();

    void send_input_report(USBReport01_t&);

    const std::vector<uint8_t>& get_report_descriptor() const override;

  private:
    static std::vector<uint8_t> _report_descriptor;
  };
}

#endif /* _DUALSHOCK4HIDINTERFACE_H */
