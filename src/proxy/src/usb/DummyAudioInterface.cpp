/**
 * @file DummyAudioInterface.cpp
 */

/**********************************************************************

  Created: 26 Jul 2021

    Copyright (C) 2021 Quividi

**********************************************************************/

#include "ControlEndpoint.h"
#include "DummyAudioInterface.h"

namespace dsremap
{
  void DummyAudioInterface::add_in_endpoint(InEndpoint*)
  {
  }

  void DummyAudioInterface::add_out_endpoint(OutEndpoint*)
  {
  }

  void DummyAudioInterface::get_cur(ControlEndpoint& ep, uint8_t, uint16_t)
  {
    ep.write((const uint8_t*)&_cur, sizeof(_cur));
  }

  void DummyAudioInterface::get_min(ControlEndpoint& ep, uint8_t, uint16_t)
  {
    uint16_t value = 0x0000;
    ep.write((const uint8_t*)&value, sizeof(value));
  }

  void DummyAudioInterface::get_max(ControlEndpoint& ep, uint8_t, uint16_t)
  {
    uint16_t value = 0x00FF;
    ep.write((const uint8_t*)&value, sizeof(value));
  }

  void DummyAudioInterface::get_res(ControlEndpoint& ep, uint8_t, uint16_t)
  {
    uint16_t value = 0x0001;
    ep.write((const uint8_t*)&value, sizeof(value));
  }

  void DummyAudioInterface::set_cur(uint8_t, uint16_t, uint16_t value)
  {
    _cur = value;
  }

  void DummyAudioInterface::on_endpoint_data(OutEndpoint&, const std::vector<uint8_t>&)
  {
  }

  void DummyAudioInterface::on_error(std::exception_ptr)
  {
  }
}
