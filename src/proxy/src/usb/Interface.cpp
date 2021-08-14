/**
 * @file Interface.cpp
 */

/**********************************************************************

  Created: 16 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include "Interface.h"

namespace dsremap
{
  Interface::Interface(unsigned int index)
    : Logger("Interface"),
      _index(index)
  {
  }

  Interface::~Interface()
  {
  }
}
