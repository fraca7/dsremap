
/**
 * @file DualsenseHIDInterface.h
 */

/**********************************************************************

  Created: 01 Sep 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUALSENSEHIDINTERFACE_H
#define _DUALSENSEHIDINTERFACE_H

#include <src/usb/HIDInterface.h>

namespace dsremap
{
  class DualsenseHIDInterface : public HIDInterface
  {
  public:
    DualsenseHIDInterface(HIDInterface::Listener&);
    ~DualsenseHIDInterface();

    const std::vector<uint8_t>& get_report_descriptor() const override;

  private:
    static std::vector<uint8_t> _report_descriptor;
  };
}

#endif /* _DUALSENSEHIDINTERFACE_H */
