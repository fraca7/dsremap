
/**
 * @file DummyAudioInterface.h
 */

/**********************************************************************

  Created: 26 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DUMMYAUDIOINTERFACE_H
#define _DUMMYAUDIOINTERFACE_H

#include <cstdint>

#include <src/usb/AudioInterface.h>

namespace dsremap
{
  class DummyAudioInterface : public AudioInterface
  {
  public:
    using AudioInterface::AudioInterface;

    void add_in_endpoint(InEndpoint*) override;
    void add_out_endpoint(OutEndpoint*) override;

    void get_cur(ControlEndpoint&, uint8_t id, uint16_t wValue) override;
    void get_min(ControlEndpoint&, uint8_t id, uint16_t wValue) override;
    void get_max(ControlEndpoint&, uint8_t id, uint16_t wValue) override;
    void get_res(ControlEndpoint&, uint8_t id, uint16_t wValue) override;

    void set_cur(uint8_t id, uint16_t wValue, uint16_t value) override;

    void on_endpoint_data(OutEndpoint&, const std::vector<uint8_t>&) override;
    void on_error(std::exception_ptr) override;

  private:
    uint16_t _cur{0x0001};
  };
}

#endif /* _DUMMYAUDIOINTERFACE_H */
