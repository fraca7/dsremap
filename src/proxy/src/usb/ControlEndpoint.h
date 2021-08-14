
/**
 * @file ControlEndpoint.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _CONTROLENDPOINT_H
#define _CONTROLENDPOINT_H

#include <src/usb/InEndpoint.h>
#include <src/utils/Application.h>

namespace dsremap
{
  class ControlEndpoint : public InEndpoint
  {
  public:
    class Listener : public Application::ErrorHandler
    {
    public:
      virtual void on_control_data_available(ControlEndpoint&) = 0;
    };

    ControlEndpoint(USBDevice& dev, int fd);
    virtual ~ControlEndpoint();

    gsize read(uint8_t* data, gsize len);

  private:
    Listener& _listener;

    static gboolean static_io_callback(int fd, GIOCondition cond, gpointer ptr);
    bool io_callback(GIOCondition cond);
  };
}

#endif /* _CONTROLENDPOINT_H */
