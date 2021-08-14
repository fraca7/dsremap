
/**
 * @file FunctionFS.h
 */

/**********************************************************************

  Created: 17 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _FUNCTIONFS_H
#define _FUNCTIONFS_H

#include <src/utils/File.h>
#include <src/utils/Logger.h>

namespace dsremap
{
  class USBDevice;

  class FunctionFS : public Logger
  {
  public:
    FunctionFS(const USBDevice&);
    ~FunctionFS();

  public:
    File configfs_path;
    File functionfs_path;
    File gadget_path;
    File config_path;
    File strings_path;
    File device_path;
    File device_link;
    File udc;
  };
}

#endif /* _FUNCTIONFS_H */
