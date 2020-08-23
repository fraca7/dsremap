
Reverse engineering
===================

Those observations come from different sources:

  * https://eleccelerator.com/wiki/index.php?title=DualShock_4
  * The Linux kernel sony-hid driver: https://github.com/torvalds/linux/blob/master/drivers/hid/hid-sony.c
  * My own experiments

The Dualshock used is a recent version (product 0x9cc).

Device descriptor
-----------------

Not much to see here.

.. code-block:: none

  Descriptor Length:	12
  Descriptor type:	01
  USB version:		0200
  Device class:		00
  Device Subclass:	00
  Device Protocol:	00
  Max.packet size:	40
  Vendor  ID:		054C
  Product ID:		09CC
  Revision ID:		0100
  Mfg.string index:	01
  Prod.string index:	02
  Serial number index:	00
  Number of conf.:	01

Configuration descriptor
------------------------

Decoded from a raw dump courtesy of https://eleccelerator.com/usbdescreqparser/

225 bytes

.. code-block:: none

  0x09,        // bLength
  0x02,        // bDescriptorType (Configuration)
  0xE1, 0x00,  // wTotalLength 225
  0x04,        // bNumInterfaces 4
  0x01,        // bConfigurationValue
  0x00,        // iConfiguration (String Index)
  0xC0,        // bmAttributes Self Powered
  0xFA,        // bMaxPower 500mA

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
  0x00, 0x00,  // wLockDelay 0

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
  0x84,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x40, 0x00,  // wMaxPacketSize 64
  0x05,        // bInterval 5 (unit depends on device speed)

  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x03,        // bEndpointAddress (OUT/H2D)
  0x03,        // bmAttributes (Interrupt)
  0x40, 0x00,  // wMaxPacketSize 64
  0x05,        // bInterval 5 (unit depends on device speed)

HID report descriptor
---------------------

.. code-block:: none

  0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
  0x09, 0x05,        // Usage (Game Pad)
  0xA1, 0x01,        // Collection (Application)
  0x85, 0x01,        //   Report ID (1)
  0x09, 0x30,        //   Usage (X)
  0x09, 0x31,        //   Usage (Y)
  0x09, 0x32,        //   Usage (Z)
  0x09, 0x35,        //   Usage (Rz)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x04,        //   Report Count (4)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x09, 0x39,        //   Usage (Hat switch)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x07,        //   Logical Maximum (7)
  0x35, 0x00,        //   Physical Minimum (0)
  0x46, 0x3B, 0x01,  //   Physical Maximum (315)
  0x65, 0x14,        //   Unit (System: English Rotation, Length: Centimeter)
  0x75, 0x04,        //   Report Size (4)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x42,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,Null State)
  0x65, 0x00,        //   Unit (None)
  0x05, 0x09,        //   Usage Page (Button)
  0x19, 0x01,        //   Usage Minimum (0x01)
  0x29, 0x0E,        //   Usage Maximum (0x0E)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x0E,        //   Report Count (14)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x20,        //   Usage (0x20)
  0x75, 0x06,        //   Report Size (6)
  0x95, 0x01,        //   Report Count (1)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x7F,        //   Logical Maximum (127)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
  0x09, 0x33,        //   Usage (Rx)
  0x09, 0x34,        //   Usage (Ry)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x02,        //   Report Count (2)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x36,        //   Report Count (54)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x85, 0x05,        //   Report ID (5)
  0x09, 0x22,        //   Usage (0x22)
  0x95, 0x1F,        //   Report Count (31)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x04,        //   Report ID (4)
  0x09, 0x23,        //   Usage (0x23)
  0x95, 0x24,        //   Report Count (36)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x02,        //   Report ID (2)
  0x09, 0x24,        //   Usage (0x24)
  0x95, 0x24,        //   Report Count (36)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x08,        //   Report ID (8)
  0x09, 0x25,        //   Usage (0x25)
  0x95, 0x03,        //   Report Count (3)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x10,        //   Report ID (16)
  0x09, 0x26,        //   Usage (0x26)
  0x95, 0x04,        //   Report Count (4)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x11,        //   Report ID (17)
  0x09, 0x27,        //   Usage (0x27)
  0x95, 0x02,        //   Report Count (2)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x12,        //   Report ID (18)
  0x06, 0x02, 0xFF,  //   Usage Page (Vendor Defined 0xFF02)
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x0F,        //   Report Count (15)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x13,        //   Report ID (19)
  0x09, 0x22,        //   Usage (0x22)
  0x95, 0x16,        //   Report Count (22)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x14,        //   Report ID (20)
  0x06, 0x05, 0xFF,  //   Usage Page (Vendor Defined 0xFF05)
  0x09, 0x20,        //   Usage (0x20)
  0x95, 0x10,        //   Report Count (16)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x15,        //   Report ID (21)
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x2C,        //   Report Count (44)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x06, 0x80, 0xFF,  //   Usage Page (Vendor Defined 0xFF80)
  0x85, 0x80,        //   Report ID (-128)
  0x09, 0x20,        //   Usage (0x20)
  0x95, 0x06,        //   Report Count (6)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x81,        //   Report ID (-127)
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x06,        //   Report Count (6)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x82,        //   Report ID (-126)
  0x09, 0x22,        //   Usage (0x22)
  0x95, 0x05,        //   Report Count (5)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x83,        //   Report ID (-125)
  0x09, 0x23,        //   Usage (0x23)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x84,        //   Report ID (-124)
  0x09, 0x24,        //   Usage (0x24)
  0x95, 0x04,        //   Report Count (4)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x85,        //   Report ID (-123)
  0x09, 0x25,        //   Usage (0x25)
  0x95, 0x06,        //   Report Count (6)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x86,        //   Report ID (-122)
  0x09, 0x26,        //   Usage (0x26)
  0x95, 0x06,        //   Report Count (6)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x87,        //   Report ID (-121)
  0x09, 0x27,        //   Usage (0x27)
  0x95, 0x23,        //   Report Count (35)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x88,        //   Report ID (-120)
  0x09, 0x28,        //   Usage (0x28)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x89,        //   Report ID (-119)
  0x09, 0x29,        //   Usage (0x29)
  0x95, 0x02,        //   Report Count (2)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x90,        //   Report ID (-112)
  0x09, 0x30,        //   Usage (0x30)
  0x95, 0x05,        //   Report Count (5)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x91,        //   Report ID (-111)
  0x09, 0x31,        //   Usage (0x31)
  0x95, 0x03,        //   Report Count (3)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x92,        //   Report ID (-110)
  0x09, 0x32,        //   Usage (0x32)
  0x95, 0x03,        //   Report Count (3)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x93,        //   Report ID (-109)
  0x09, 0x33,        //   Usage (0x33)
  0x95, 0x0C,        //   Report Count (12)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x94,        //   Report ID (-108)
  0x09, 0x34,        //   Usage (0x34)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA0,        //   Report ID (-96)
  0x09, 0x40,        //   Usage (0x40)
  0x95, 0x06,        //   Report Count (6)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA1,        //   Report ID (-95)
  0x09, 0x41,        //   Usage (0x41)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA2,        //   Report ID (-94)
  0x09, 0x42,        //   Usage (0x42)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA3,        //   Report ID (-93)
  0x09, 0x43,        //   Usage (0x43)
  0x95, 0x30,        //   Report Count (48)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA4,        //   Report ID (-92)
  0x09, 0x44,        //   Usage (0x44)
  0x95, 0x0D,        //   Report Count (13)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF0,        //   Report ID (-16)
  0x09, 0x47,        //   Usage (0x47)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF1,        //   Report ID (-15)
  0x09, 0x48,        //   Usage (0x48)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF2,        //   Report ID (-14)
  0x09, 0x49,        //   Usage (0x49)
  0x95, 0x0F,        //   Report Count (15)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA7,        //   Report ID (-89)
  0x09, 0x4A,        //   Usage (0x4A)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA8,        //   Report ID (-88)
  0x09, 0x4B,        //   Usage (0x4B)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA9,        //   Report ID (-87)
  0x09, 0x4C,        //   Usage (0x4C)
  0x95, 0x08,        //   Report Count (8)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xAA,        //   Report ID (-86)
  0x09, 0x4E,        //   Usage (0x4E)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xAB,        //   Report ID (-85)
  0x09, 0x4F,        //   Usage (0x4F)
  0x95, 0x39,        //   Report Count (57)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xAC,        //   Report ID (-84)
  0x09, 0x50,        //   Usage (0x50)
  0x95, 0x39,        //   Report Count (57)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xAD,        //   Report ID (-83)
  0x09, 0x51,        //   Usage (0x51)
  0x95, 0x0B,        //   Report Count (11)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xAE,        //   Report ID (-82)
  0x09, 0x52,        //   Usage (0x52)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xAF,        //   Report ID (-81)
  0x09, 0x53,        //   Usage (0x53)
  0x95, 0x02,        //   Report Count (2)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xB0,        //   Report ID (-80)
  0x09, 0x54,        //   Usage (0x54)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xE0,        //   Report ID (-32)
  0x09, 0x57,        //   Usage (0x57)
  0x95, 0x02,        //   Report Count (2)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xB3,        //   Report ID (-77)
  0x09, 0x55,        //   Usage (0x55)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xB4,        //   Report ID (-76)
  0x09, 0x55,        //   Usage (0x55)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xB5,        //   Report ID (-75)
  0x09, 0x56,        //   Usage (0x56)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xD0,        //   Report ID (-48)
  0x09, 0x58,        //   Usage (0x58)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xD4,        //   Report ID (-44)
  0x09, 0x59,        //   Usage (0x59)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection

Sony likes its vendor-defined stuff.

Input report structure
----------------------

This is the USB 0x01 report descriptor. Over BT, the report ID is 0x11 and it's followed by two bytes (0xc0 0x00), so all subsequent offsets must be adjusted.

+--------+--------+--------------------------------------------------------------------------+
+ Offset + Type   + Meaning                                                                  +
+========+========+==========================================================================+
+ 0      + uint8  + Report descriptor                                                        +
+--------+--------+--------------------------------------------------------------------------+
+ 1      + uint8  + Left pad X (0x00 is left, 0xFF right)                                    +
+--------+--------+--------------------------------------------------------------------------+
+ 2      + uint8  + Left pad Y (0x00 is up, 0xFF down)                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 3      + uint8  + Right pad X                                                              +
+--------+--------+--------------------------------------------------------------------------+
+ 4      + uint8  + Right pad Y                                                              +
+--------+--------+--------------------------------------------------------------------------+
+ 5      + uint4  + DPAD (bits 0-3). 0x8 is released, 0x0 is N, 0x1 is NE, etc.              +
+--------+--------+--------------------------------------------------------------------------+
+ 5      + bool   + Square (bit 4)                                                           +
+--------+--------+--------------------------------------------------------------------------+
+ 5      + bool   + Cross (bit 5)                                                            +
+--------+--------+--------------------------------------------------------------------------+
+ 5      + bool   + Circle (bit 6)                                                           +
+--------+--------+--------------------------------------------------------------------------+
+ 5      + bool   + Triangle (bit  7)                                                        +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + L1 (bit 0)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + R1 (bit 1)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + L2 (bit 2)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + R2 (bit 3)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + Share (bit 4)                                                            +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + Options (bit 5)                                                          +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + L3 (bit 6)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + bool   + R3 (bit 7)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 7      + bool   + PS (bit 0)                                                               +
+--------+--------+--------------------------------------------------------------------------+
+ 7      + bool   + TPad (bit 1)                                                             +
+--------+--------+--------------------------------------------------------------------------+
+ 7      + uint6  + Incremental counter (bits 2-7).                                          +
+--------+--------+--------------------------------------------------------------------------+
+ 8      + uint8  + L2 value (0x00 = released, 0xFF = fully pressed)                         +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + uint8  + R2 value                                                                 +
+--------+--------+--------------------------------------------------------------------------+
+ 10-11  + uint16 + Timestamp in 5.33 microseconds units                                     +
+--------+--------+--------------------------------------------------------------------------+
+ 12     + uint8  + Unknown, maybe battery level, but this would be redundant with offset 23 +
+--------+--------+--------------------------------------------------------------------------+
+ 13-14  + int16  + Gyro X raw value                                                         +
+--------+--------+--------------------------------------------------------------------------+
+ 15-16  + int16  + Gyro Y raw value                                                         +
+--------+--------+--------------------------------------------------------------------------+
+ 17-18  + int16  + Gyro Z raw value                                                         +
+--------+--------+--------------------------------------------------------------------------+
+ 19-20  + int16  + Accelerometer X raw value                                                +
+--------+--------+--------------------------------------------------------------------------+
+ 21-22  + int16  + Accelerometer Y raw value                                                +
+--------+--------+--------------------------------------------------------------------------+
+ 23-24  + int16  + Accelerometer Z raw value                                                +
+--------+--------+--------------------------------------------------------------------------+
+ 25-29  +        + Unknown, set to 0x00                                                     +
+--------+--------+--------------------------------------------------------------------------+
+ 30     + uint4  + Battery level (bits 0-3). See below.                                     +
+--------+--------+--------------------------------------------------------------------------+
+ 30     + bool   + Cable state (bit 4)                                                      +
+--------+--------+--------------------------------------------------------------------------+
+ 30     +        + Unknown (bits 5-7)                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 31-32  +        + Unknown                                                                  +
+--------+--------+--------------------------------------------------------------------------+
+ 33     + uint8  + Touch event count                                                        +
+--------+--------+--------------------------------------------------------------------------+
+ 34     +        + Unknown                                                                  +
+--------+--------+--------------------------------------------------------------------------+

About the battery level at offset 23: according to the sony-hid driver sources, the range is 0..9 when running on battery, 0..10 when connected to power. More than 10 and connected means battery fully charged.

The rest of the report starting at offset 35 is an array of touchpad events. Each event has the following structure:

+--------+-------+-----------------------------------------------+
+ Offset + Type  + Meaning                                       +
+========+=======+===============================================+
+ 0      + uint8 + Timestamp                                     +
+--------+-------+-----------------------------------------------+
+ 1      + uint7 + Previous event counter (bits 0-6)             +
+--------+-------+-----------------------------------------------+
+ 1      + bool  + Previous event finger down (bit 7, 0=touch)   +
+--------+-------+-----------------------------------------------+
+ 2-4    +       + Previous position, in 12 bits coordinates X/Y +
+--------+-------+-----------------------------------------------+
+ 5      + uint7 + Current event counter (bits 0-6)              +
+--------+-------+-----------------------------------------------+
+ 5      + bool  + Current event finger down (bit 7)             +
+--------+-------+-----------------------------------------------+
+ 6-8    +       + Current position                              +
+--------+-------+-----------------------------------------------+

.. note:: The maximum number of touch events is 3 over USB and 4 over
	  BT. In each case the rest of the report (2 remaining bytes
	  for USB, 5 for BT) is "unknown".

Remarks
#######

The configuration descriptor defines 3 audio interfaces (0, 1, 2) with 1 and 2 having alternate settings. The PS4 is very picky about this. Only spoofing the HID interface will seem to work, in the sense that the "boot" sequence will be identical, but the PS4 will not acknowledge the controller completely and will get stuck on the "Press PS" screen.

So, when spoofing the DualShock, the device must send the exact same USB descriptor. The actual audio interfaces do not need to be implemented when dealing with the PS4. On the other hand, mac OS (and probably others) will reset the device if they aren't implemented. That is why Host.cpp has some logic to detect if it's plugged to a PS4 or a PC; in the latter case audio interfaces are not included in the USB descriptor.

Boot sequence (PS4)
-------------------

This is what happens when a Dualshock is plugged to the PS4.

GET_REPORT 0x02
###############

This report contains IMU calibration data. When the Dualshock is paired via Bluetooth, this has the additional side effect of changing the input report type from the default 0x01 (pretty limited, with only buttons and triggers, see DS4Structs.h) to 0x11, which is identical in structure to the USB input reports 0x01, only larger (4 possible touch events instead of 3).

Everything is little-endian.

+--------+-------+----------------------------------------+
| Offset + Type  + Meaning                                +
+========+=======+========================================+
+ 0      + uint8 + Report ID (0xa3)                       +
+--------+-------+----------------------------------------+
+ 1-2    + int16 + Gyroscope X bias                       +
+--------+-------+----------------------------------------+
+ 3-4    + int16 + Gyroscope Y bias                       +
+--------+-------+----------------------------------------+
+ 5-6    + int16 + Gyroscope Z bias                       +
+--------+-------+----------------------------------------+
+ 7-8    + int16 + Gyroscope X maximum                    +
+--------+-------+----------------------------------------+
+ 9-10   + int16 + Gyroscope X minimum                    +
+--------+-------+----------------------------------------+
+ 11-12  + int16 + Gyroscope Y maximum                    +
+--------+-------+----------------------------------------+
+ 13-14  + int16 + Gyroscope Y minimum                    +
+--------+-------+----------------------------------------+
+ 15-16  + int16 + Gyroscope Z maximum                    +
+--------+-------+----------------------------------------+
+ 17-18  + int16 + Gyroscope Z minimum                    +
+--------+-------+----------------------------------------+
+ 18-19  + int16 + Gyroscope speed maximum                +
+--------+-------+----------------------------------------+
+ 20-21  + int16 + Gyroscope speed minimum                +
+--------+-------+----------------------------------------+
+ 22-23  + int16 + Accelerometer X maximum                +
+--------+-------+----------------------------------------+
+ 24-25  + int16 + Accelerometer X minimum                +
+--------+-------+----------------------------------------+
+ 26-27  + int16 + Accelerometer Y maximum                +
+--------+-------+----------------------------------------+
+ 28-29  + int16 + Accelerometer Y minimum                +
+--------+-------+----------------------------------------+
+ 30-31  + int16 + Accelerometer Z maximum                +
+--------+-------+----------------------------------------+
+ 32-33  + int16 + Accelerometer Z minimum                +
+--------+-------+----------------------------------------+
+ 34-35  +       + Unknown                                +
+--------+-------+----------------------------------------+

See :ref:`use_calibration`

GET_REPORT 0xa3
###############

This contains some time of manufacture information, part of it in plain text. Not entirely deciphered though. It's identical every time (for a given controller).

GET_REPORT 0x12
###############

This contains information about the Dualshock pairing "state".

+--------+-------+---------------------------------------------------------+
+ Offset + Type  + Meaning                                                 +
+========+=======+=========================================================+
+ 0      + uint8 + Report ID (0x12)                                        +
+--------+-------+---------------------------------------------------------+
+ 1-6    +       + BT address of the Dualshock                             +
+--------+-------+---------------------------------------------------------+
+ 7-9    +       + Seems constant to 0x08 0x25 0x00                        +
+--------+-------+---------------------------------------------------------+
+ 10-15  +       + BT address of the last device the DualShock paired with +
+--------+-------+---------------------------------------------------------+

The last 6 bytes are set to 0x00 if the Dualshock was never paired.

SET_REPORT 0x13
###############

This only happens if the PS4 BT address in report 0x12 does not match the PS4's. It contains the PS4 BT address, and the link key for BT encryption.

+--------+-------+-----------------------+
+ Offset + Type  + Meaning               +
+========+=======+=======================+
+ 0      + uint8 + Report ID (0x13)      +
+--------+-------+-----------------------+
+ 1-6    +       + Host (PS4) BT address +
+--------+-------+-----------------------+
+ 7-15   +       + Link key              +
+--------+-------+-----------------------+

For some reason, after this I get write timeouts on the interrupt IN endpoint to the host, though the control endpoint seems unaffected. This is probably a bug on my side.

SET_REPORT 0x14
###############

+--------+-------+-----------------------+
+ Offset + Type  + Meaning               +
+========+=======+=======================+
+ 0      + uint8 + Report ID (0x14)      +
+--------+-------+-----------------------+
+ 1      + uint8 + Command (see below)   +
+--------+-------+-----------------------+
+ 2-15   +       + Filled with NUL       +
+--------+-------+-----------------------+

'Command' may take the following values:

  * 0x01: Pair with the PS4
  * 0x02: Unpair

See also :ref:`bluetooth_mandatory`

After a SET_REPORT 0x13, a SET_REPORT 0x14 with value 0x02 is
immediately sent to disconnect the DS from its current host.

After pressing PS
-----------------

SET_REPORT 0x14
###############

Immediately after pressing PS, the PS4 asks the Dualshock to connect via Bluetooth using this report (with first byte 0x01, see above).

After this, the host sends regular data over the interrupt OUT endpoint (to set the LED colors/rumble/etc) and the periodic authentication challenge starts.

Authentication challenge
------------------------

The host sends several SET_REPORT 0xf0 containing challenge data, then checks challenge response availability through GET_REPORT 0xf2, and finally collects the response with GET_REPORT 0xf1. This is all explained in https://eleccelerator.com/wiki/index.php?title=DualShock_4#0xf1

.. _use_calibration:

Boot sequence (mac OS)
----------------------

  * GET_REPORT 0xa3 Manufacturer info
  * GET_REPORT 0x12 Pairing state

Boot sequence (Linux)
---------------------

  * GET_REPORT 0x81 Controller MAC address
  * GET_REPORT 0x02 IMU calibration data
  * GET_REPORT 0xa3 Manufacturer info

The 0x81 report:

+--------+-------+---------------------------------------------------------+
+ Offset + Type  + Meaning                                                 +
+========+=======+=========================================================+
+ 0      + uint8 + Report ID (0x81)                                        +
+--------+-------+---------------------------------------------------------+
+ 1-6    +       + BT address of the Dualshock                             +
+--------+-------+---------------------------------------------------------+

Boot sequence (Windows)
-----------------------

Windows does not send anything after getting the report descriptor.

IMU calibration
---------------

The 0x02 report contains IMU calibration data as explained above; calibrated values can be computed from the raw ones using the following formulae.

Gyroscope
#########

.. code-block:: none

   speed = (max_speed + min_speed) / 2
   value = speed * (raw_value - bias) / (max_value - min_value)

In degrees/s.

.. note::
   There's a 2 factor in the Linux driver, i.e. max_speed and min_speed are added instead of averaged. Either there's something I don't get, or the factor is taken care of in the resolution constant, or it's a bug.

Accelerometer
#############

.. code-block:: none

   oneG = (max_value - min_value) / 2
   value = (value - oneG) / oneG

In Gs.

.. _bluetooth_mandatory:

Bluetooth is mandatory
----------------------

Unfortunately, even when the PS4 is set to communicate with the Dualshock through USB using the appropriate system setting, nothing will work if the Dualshock cannot establish a Bluetooth connection to the PS4. Here is the little experiment I did that proves this:

  * Paired the Dualshock with a Linux PC, then disconnect it.
  * Changed the code to

    1. Spoof the 0x12 GET_REPORT so that the PS4 thinks the DS is paired with it, by hardcoding the PS4 address into it
    2. Mapped the Cross button to PS (to make sure the DS simply doesn't connect by itself when pressing PS)

Then I plug the DS to the PS4 through the Arduino. Nothing happens until I press Cross on the DS; then the 0x14 0x01 ... SET_REPORT happens, and the DS automatically connects to the PC. The PS4 does not react (stuck on "Press PS" screen").

This shows that the PS4 will not acknowledge the DS over USB if it's not, at the same time, connected through BT.

Unfortunately this means that setting up the Arduino to work untethered from the DS would be quite complicated: we would need two host shields (not even sure that's possible), and two BT dongles, so that we can close the loop to the PS4 using the second one. Not to mention there's not enough program memory in the Leonardo for this...

It is possible though that only the initial connection is mandatory, and the PS4 would happily go on after a disconnection. To test this I'll need to find out how to switch the BT dongle mode, in order to

  1. Handle the boot sequence on its own and use the dongle to connect to the PS4, thus unblocking the situation
  2. Only then switch the dongle to "listen" mode and wait for the DS to connect

Or maybe a single BT dongle can be used to communicate with both the DS and the PS4 ? I don't know enough to tell.
