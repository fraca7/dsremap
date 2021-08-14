/**
 * @file DescriptorParser.cpp
 */

/**********************************************************************

  Created: 09 Aug 2021

    Copyright (C) 2021 Quividi

**********************************************************************/

#include <src/utils/format.h>

#include "DescriptorParser.h"

namespace dsremap
{
  DescriptorParser::DescriptorParser(USBDevice& dev)
    : _dev(dev)
  {
  }

  void DescriptorParser::parse(const std::vector<uint8_t>& desc)
  {
    Interface* current_if = nullptr;
    unsigned int current_ep = 0;

    unsigned int offset = 0;
    while (offset < desc.size()) {
      uint8_t bLength = desc[offset];
      if ((bLength + offset > desc.size()) || (bLength == 0))
        throw std::runtime_error(format("Invalid descriptor length 0x{:02x} at offset {} (size {})", bLength, offset, desc.size()));

      switch (desc[offset + 1]) {
        case 0x04: // Interface
          current_if = on_interface(*((Interface::interface_descriptor_t*)&desc[offset + 2]));
          break;
        case 0x05: // Endpoint
        {
          on_endpoint(current_if, ++current_ep, desc[offset + 2]);
          break;
        }
        default:
        {
          std::vector<uint8_t> subdesc;
          copy(desc.begin() + offset, desc.begin() + offset + bLength, back_inserter(subdesc));
          on_class_descriptor(current_if, subdesc);
          break;
        }
      }

      offset += bLength;
    }
  }
}
