/**
 * @file FunctionFS.cpp
 */

/**********************************************************************

  Created: 17 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <src/utils/format.h>

#include "USBDevice.h"
#include "FunctionFS.h"

#define CONFIGFS_MP "/mnt/configfs"
#define FUNCTIONFS_MP "/mnt/functionfs"
#define GADGET_PATH "usb_gadget"

namespace dsremap
{
  FunctionFS::FunctionFS(const USBDevice& dev)
    : Logger("FunctionFS"),
      configfs_path(CONFIGFS_MP),
      functionfs_path(format("{}_{}", FUNCTIONFS_MP, dev.name())),
      gadget_path(configfs_path, format("{}/{}", GADGET_PATH, dev.name())),
      config_path(gadget_path, "configs/c.1"),
      strings_path(gadget_path, "strings/0x409"),
      device_path(gadget_path, format("functions/ffs.{}", dev.name())),
      device_link(config_path, format("ffs.{}", dev.name())),
      udc("")
  {
    if (!configfs_path.exists()) {
      debug("Creating \"{}\"", configfs_path.as_string());
      configfs_path.mkdir();
    }

    if (!functionfs_path.exists()) {
      debug("Creating \"{}\"", functionfs_path.as_string());
      functionfs_path.mkdir();
    }

    debug("Mounting configfs");
    configfs_path.mount("configfs");
    try {
      debug("Creating \"{}\"", gadget_path.as_string());
      gadget_path.mkdir();
      try {
        debug("Writing device information");
        File(gadget_path, "bDeviceClass").echo("0x00");
        File(gadget_path, "bDeviceSubClass").echo("0x00");
        File(gadget_path, "bDeviceProtocol").echo("0x00");
        File(gadget_path, "bMaxPacketSize0").echo(format("0x{:02x}", dev.max_packet_size()));
        File(gadget_path, "idVendor").echo(format("0x{:04x}", dev.vendor_id()));
        File(gadget_path, "idProduct").echo(format("0x{:04x}", dev.product_id()));
        File(gadget_path, "bcdDevice").echo("0x100");

        debug("Creating config path");
        config_path.mkdir();
        try {
          File(config_path, "bmAttributes").echo("0xc0");
          File(config_path, "MaxPower").echo("500");

          debug("Creating strings path");
          strings_path.mkdir();
          try {
            File(strings_path, "manufacturer").echo(dev.manufacturer());
            File(strings_path, "product").echo(dev.product());

            debug("Creating device path");
            device_path.mkdir();
            try {
              debug("Creating device link");
              device_path.symlink(device_link);
              try {
                debug("Mounting functionfs");
                functionfs_path.mount("functionfs", dev.name());
                try {
                  bool found = false;
                  for (const File& filename : File("/sys/class/udc").ls()) {
                    debug("Found UDC: \"{}\"", filename.as_string());
                    udc = filename;
                    found = true;
                    break;
                  }

                  if (!found)
                    throw std::runtime_error("UDC not found");
                } catch (...) {
                  debug("Unmounting functionfs");
                  functionfs_path.unmount();
                  throw;
                }
              } catch (...) {
                debug("Deleting device link");
                device_link.unlink();
                throw;
              }
            } catch (...) {
              debug("Deleting device path");
              device_path.rmdir();
              throw;
            }
          } catch (...) {
            debug("Deleting strings path");
            strings_path.rmdir();
            throw;
          }
        } catch (...) {
          debug("Deleting config path");
          config_path.rmdir();
          throw;
        }
      } catch (...) {
        debug("Deleting \"{}\"", gadget_path.as_string());
        gadget_path.rmdir();
        throw;
      }
    } catch (...) {
      debug("Unmounting configfs");
      configfs_path.unmount();
      throw;
    }
  }

  FunctionFS::~FunctionFS()
  {
    debug("Unmounting functionfs");
    functionfs_path.unmount();
    debug("Deleting device link");
    device_link.unlink();
    debug("Deleting device path");
    device_path.rmdir();
    debug("Deleting strings path");
    strings_path.rmdir();
    debug("Deleting config path");
    config_path.rmdir();
    debug("Deleting \"{}\"", gadget_path.as_string());
    gadget_path.rmdir();
    debug("Unmounting configfs");
    configfs_path.unmount();
  }
}
