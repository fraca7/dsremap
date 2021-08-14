
/**
 * @file DescriptorParser.h
 */

/**********************************************************************

  Created: 09 Aug 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DESCRIPTORPARSER_H
#define _DESCRIPTORPARSER_H

#include <vector>
#include <cstdint>

#include <src/usb/Interface.h>

namespace dsremap
{
  class USBDevice;

  /**
   * Utility class for parsing USB descriptors
   */
  class DescriptorParser
  {
  public:
    DescriptorParser(USBDevice&);

    void parse(const std::vector<uint8_t>&);

  protected:
    USBDevice& _dev;

    virtual Interface* on_interface(const Interface::interface_descriptor_t& descriptor) = 0;
    virtual void on_endpoint(Interface*, unsigned int global_index, uint8_t index) = 0;
    virtual void on_class_descriptor(Interface*, const std::vector<uint8_t>& descriptor) = 0;
  };
}

#endif /* _DESCRIPTORPARSER_H */
