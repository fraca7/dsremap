
#ifndef _DESCRIPTORS_H
#define _DESCRIPTORS_H

#include "Config.h"

#include "LUFAConfig.h"

#include <LUFA.h>

#define DEVICE_INTERFACE_NUMBER 3

#define DEVICE_EPADDR_IN (4 | ENDPOINT_DIR_IN)
#define DEVICE_EPADDR_OUT (3 | ENDPOINT_DIR_OUT)
#define DEVICE_EP_SIZE 0x40

typedef struct {
  USB_Descriptor_Configuration_Header_t Config;
  USB_Descriptor_Interface_t HID_Interface;
  USB_HID_Descriptor_HID_t HID;
  USB_Descriptor_Endpoint_t HID_ReportINEndpoint;
  USB_Descriptor_Endpoint_t HID_ReportOUTEndpoint;
} USB_Descriptor_Configuration_t;

enum StringDescriptors_t
{
  STRING_ID_Language     = 0,
  STRING_ID_Manufacturer = 1,
  STRING_ID_Product      = 2,
};

// Add a pointer to the HIDClass instance in the classinfo, used in
// the callbacks to forward to the instance

class Host;

typedef struct {
  USB_ClassInfo_HID_Device_t USB;
  Host *pHost;
} ClassInfo_t;

extern ClassInfo_t GlobalInterface;

#endif /* _DESCRIPTORS_H */
