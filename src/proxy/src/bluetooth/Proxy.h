
/**
 * @file Proxy.h
 */

/**********************************************************************

  Created: 09 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _PROXY_H
#define _PROXY_H

#include <vector>
#include <cstdint>

#include <glib.h>

#include <src/utils/Logger.h>
#include <src/utils/Application.h>

namespace dsremap
{
  class Proxy : public Logger,
                public Application::Component
  {
  public:
    Proxy(Application&, int fd_0x11, int fd_0x13);
    ~Proxy();

    void get_report(uint8_t id, uint16_t len);

    void write_out_data(std::vector<uint8_t>&);
    void write_int_data(std::vector<uint8_t>&);

  protected:
    virtual void got_control_data(const std::vector<uint8_t>&) = 0;
    virtual void got_interrupt_data(const std::vector<uint8_t>&) = 0;

  private:
    int _fd_0x11;
    int _fd_0x13;

    static gboolean static_io_callback(int, GIOCondition, gpointer);
    bool io_callback(int);

    void write_data(int, std::vector<uint8_t>&);
  };
}

#endif /* _PROXY_H */
