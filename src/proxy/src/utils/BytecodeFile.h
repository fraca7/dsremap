
/**
 * @file BytecodeFile.h
 */

/**********************************************************************

  Created: 04 sept. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _BYTECODEFILE_H
#define _BYTECODEFILE_H

#include <vector>
#include <cstdint>

#include <src/utils/Logger.h>

namespace dsremap
{
  class BytecodeFile : public Logger
  {
  public:
    BytecodeFile();

    bool exists() const;

    uint16_t stacksize() const;
    uint8_t* bytecode() const;

  private:
    std::vector<uint8_t> _bytecode;
    uint16_t _stacksize;
  };
}

#endif /* _BYTECODEFILE_H */
