/**
 * @file Dualshock4.cpp
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <src/utils/format.h>

#include "Dualshock4.h"

namespace dsremap
{
  Dualshock4::Dualshock4(USBDevice::Listener& dev_listener, HIDInterface::Listener& hid_listener)
    : SonyController<Dualshock4HIDInterface>(dev_listener, hid_listener)
  {
  }

  const std::vector<uint8_t>& Dualshock4::get_descriptor() const
  {
    return _usb_descriptor;
  }

  std::vector<uint8_t> Dualshock4::_usb_descriptor = {
    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x00,        // bInterfaceNumber 0
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x01,        // bInterfaceSubClass (Audio Control)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> HEADER)
    0x00, 0x01,  // bcdADC 1.00
    0x47, 0x00,  // wTotalLength 71
    0x02,        // binCollection 0x02
    0x01,        // baInterfaceNr 1
    0x02,        // baInterfaceNr 2

    0x0C,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
    0x01,        // bTerminalID
    0x01, 0x01,  // wTerminalType (USB Streaming)
    0x06,        // bAssocTerminal
    0x02,        // bNrChannels 2
    0x03, 0x00,  // wChannelConfig (Left and Right Front)
    0x00,        // iChannelNames
    0x00,        // iTerminal

    0x0A,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
    0x02,        // bUnitID
    0x01,        // bSourceID
    0x01,        // bControlSize 1
    0x03, 0x00,  // bmaControls[0] (Mute,Volume)
    0x00, 0x00,  // bmaControls[1] (None)

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
    0x03,        // bTerminalID
    0x02, 0x04,  // wTerminalType (Headset)
    0x04,        // bAssocTerminal
    0x02,        // bSourceID
    0x00,        // iTerminal

    0x0C,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
    0x04,        // bTerminalID
    0x02, 0x04,  // wTerminalType (Headset)
    0x03,        // bAssocTerminal
    0x01,        // bNrChannels 1
    0x00, 0x00,  // wChannelConfig
    0x00,        // iChannelNames
    0x00,        // iTerminal

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
    0x05,        // bUnitID
    0x04,        // bSourceID
    0x01,        // bControlSize 1
    0x03, 0x00,  // bmaControls[0] (Mute,Volume)
    0x00,        // iFeature

    0x09,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
    0x06,        // bTerminalID
    0x01, 0x01,  // wTerminalType (USB Streaming)
    0x01,        // bAssocTerminal
    0x05,        // bSourceID
    0x00,        // iTerminal

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x01,        // bInterfaceNumber 1
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x01,        // bInterfaceNumber 1
    0x01,        // bAlternateSetting
    0x01,        // bNumEndpoints 1
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x07,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
    0x01,        // bTerminalLink
    0x01,        // bDelay 1
    0x01, 0x00,  // wFormatTag (PCM)

    0x0B,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
    0x01,        // bFormatType 1
    0x02,        // bNrChannels (Stereo)
    0x02,        // bSubFrameSize 2
    0x10,        // bBitResolution 16
    0x01,        // bSamFreqType 1
    0x00, 0x7D, 0x00,  // tSamFreq[1] 32000 Hz

    0x09,        // bLength
    0x05,        // bDescriptorType (See Next Line)
    0x01,        // bEndpointAddress (OUT/H2D)
    0x09,        // bmAttributes (Isochronous, Adaptive, Data EP)
    0x84, 0x00,  // wMaxPacketSize 132
    0x01,        // bInterval 1 (unit depends on device speed)
    0x00,        // bRefresh
    0x00,        // bSyncAddress

    0x07,        // bLength
    0x25,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
    0x00,        // bmAttributes (None)
    0x00,        // bLockDelayUnits
    0x00, 0x00,  // wLockDelay 0

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x02,        // bInterfaceNumber 2
    0x00,        // bAlternateSetting
    0x00,        // bNumEndpoints 0
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x02,        // bInterfaceNumber 2
    0x01,        // bAlternateSetting
    0x01,        // bNumEndpoints 1
    0x01,        // bInterfaceClass (Audio)
    0x02,        // bInterfaceSubClass (Audio Streaming)
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x07,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_INTERFACE -> AS_GENERAL)
    0x06,        // bTerminalLink
    0x01,        // bDelay 1
    0x01, 0x00,  // wFormatTag (PCM)

    0x0B,        // bLength
    0x24,        // bDescriptorType (See Next Line)
    0x02,        // bDescriptorSubtype (CS_INTERFACE -> FORMAT_TYPE)
    0x01,        // bFormatType 1
    0x01,        // bNrChannels (Mono)
    0x02,        // bSubFrameSize 2
    0x10,        // bBitResolution 16
    0x01,        // bSamFreqType 1
    0x80, 0x3E, 0x00,  // tSamFreq[1] 16000 Hz

    0x09,        // bLength
    0x05,        // bDescriptorType (See Next Line)
    0x82,        // bEndpointAddress (IN/D2H)
    0x05,        // bmAttributes (Isochronous, Async, Data EP)
    0x22, 0x00,  // wMaxPacketSize 34
    0x01,        // bInterval 1 (unit depends on device speed)
    0x00,        // bRefresh
    0x00,        // bSyncAddress

    0x07,        // bLength
    0x25,        // bDescriptorType (See Next Line)
    0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
    0x00,        // bmAttributes (None)
    0x00,        // bLockDelayUnits
    0x00, 0x00,   // wLockDelay 0

    0x09,        // bLength
    0x04,        // bDescriptorType (Interface)
    0x03,        // bInterfaceNumber 3
    0x00,        // bAlternateSetting
    0x02,        // bNumEndpoints 2
    0x03,        // bInterfaceClass
    0x00,        // bInterfaceSubClass
    0x00,        // bInterfaceProtocol
    0x00,        // iInterface (String Index)

    0x09,        // bLength
    0x21,        // bDescriptorType (HID)
    0x11, 0x01,  // bcdHID 1.11
    0x00,        // bCountryCode
    0x01,        // bNumDescriptors
    0x22,        // bDescriptorType[0] (HID)
    0xFB, 0x01,  // wDescriptorLength[0] 507

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x81,        // bEndpointAddress (IN/D2H)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x05,        // bInterval 5 (unit depends on device speed)

    0x07,        // bLength
    0x05,        // bDescriptorType (Endpoint)
    0x02,        // bEndpointAddress (OUT/H2D)
    0x03,        // bmAttributes (Interrupt)
    0x40, 0x00,  // wMaxPacketSize 64
    0x05,        // bInterval 5 (unit depends on device speed)
  };
}
