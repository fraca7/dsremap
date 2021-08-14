
/**
 * @file InEndpoint.h
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _INENDPOINT_H
#define _INENDPOINT_H

#include <cstdint>

#include <src/usb/Endpoint.h>

namespace dsremap
{
  class InEndpoint : public Endpoint
  {
  public:
    using Endpoint::Endpoint;

    gsize write(const uint8_t* data, gsize len);

    void shutdown() override;
  };
}

#endif /* _INENDPOINT_H */
