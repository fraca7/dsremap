
Reverse engineering
===================

Those observations come from different sources:

  * https://eleccelerator.com/wiki/index.php?title=DualShock_4
  * The Linux kernel sony-hid driver: https://github.com/torvalds/linux/blob/master/drivers/hid/hid-sony.c
  * My own experiments

The Dualshock used is a recent version (product 0x9cc).

PS4/Dualshock
~~~~~~~~~~~~~

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
+ 0      + uint8  + Report ID                                                                +
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

Known feature reports
---------------------

GET_REPORT 0x02
###############

This report contains IMU calibration data. When the Dualshock is paired via Bluetooth, this has the additional side effect of changing the input report type from the default 0x01 (pretty limited, with only buttons and triggers, see DS4Structs.h) to 0x11, which is identical in structure to the USB input reports 0x01, only larger (4 possible touch events instead of 3).

Everything is little-endian.

+--------+-------+----------------------------------------+
| Offset + Type  + Meaning                                +
+========+=======+========================================+
+ 0      + uint8 + Report ID (0x02)                       +
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
+ 34-36  +       + Unknown                                +
+--------+-------+----------------------------------------+

See :ref:`use_calibration`

GET_REPORT 0xa3
###############

This contains some time of manufacture information, part of it in plain text. Not entirely deciphered though. There is a human-readable date and time. According to the Linux kernel driver, the hardware version of the controller lies at offset 35 and the firmware version at offset 41 (both little-endian 16 bits unsigned).

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

GET_REPORT 0x81
###############

This contains the Dualshock's BT address.

+--------+-------+---------------------------------------------------------+
+ Offset + Type  + Meaning                                                 +
+========+=======+=========================================================+
+ 0      + uint8 + Report ID (0x81)                                        +
+--------+-------+---------------------------------------------------------+
+ 1-6    +       + BT address of the Dualshock                             +
+--------+-------+---------------------------------------------------------+

SET_REPORT 0x13
###############

This contains the PS4 BT address, and the link key for BT encryption.

+--------+-------+-----------------------+
+ Offset + Type  + Meaning               +
+========+=======+=======================+
+ 0      + uint8 + Report ID (0x13)      +
+--------+-------+-----------------------+
+ 1-6    +       + Host (PS4) BT address +
+--------+-------+-----------------------+
+ 7-22   +       + Link key              +
+--------+-------+-----------------------+

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

Authentication challenge
########################

The host sends several SET_REPORT 0xf0 containing challenge data, then checks challenge response availability through GET_REPORT 0xf2, and finally collects the response with GET_REPORT 0xf1. This is all explained in https://eleccelerator.com/wiki/index.php?title=DualShock_4#0xf1

Boot sequence
-------------

When the Dualshock is plugged to the Playstation, the following happens:

  * GET_REPORT 0x02
  * GET_REPORT 0xa3
  * GET_REPORT 0x12

Then, if the paired address from report 0x12 did not match the PS4's:

  * SET_REPORT 0x13
  * SET_REPORT 0x14 with value 0x02

Then, after pressing PS

  * SET_REPORT 0x14 with value 0x01

Interrupt OUT reports start coming in after this (actually after the Dualshock is connected through Bluetooth).

.. _use_calibration:

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

Even when the PS4 is configured to communicate with the Dualshock through USB using the appropriate system setting, it *has* to connect through Bluetooth after receiving the 0x14 SET_REPORT, or the PS4 will not acknowledge it.

PS5/DualSense
~~~~~~~~~~~~~

Device descriptor
-----------------

.. code-block:: none

  Descriptor Length:	12
  Descriptor type:	01
  USB version:		0200
  Device class:		00
  Device Subclass:	00
  Device Protocol:	00
  Max.packet size:	40
  Vendor  ID:		054C
  Product ID:		0CE6
  Revision ID:		0100
  Mfg.string index:	01
  Prod.string index:	02
  Serial number index:	00
  Number of conf.:	01

Configuration descriptor
------------------------

227 bytes

.. code-block:: none

  0x09,        // bLength
  0x02,        // bDescriptorType (Configuration)
  0xE3, 0x00,  // wTotalLength 227
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
  0x49, 0x00,  // wTotalLength 73
  0x02,        // binCollection 0x02
  0x01,        // baInterfaceNr 1
  0x02,        // baInterfaceNr 2

  0x0C,        // bLength
  0x24,        // bDescriptorType (See Next Line)
  0x02,        // bDescriptorSubtype (CS_INTERFACE -> INPUT_TERMINAL)
  0x01,        // bTerminalID
  0x01, 0x01,  // wTerminalType (USB Streaming)
  0x06,        // bAssocTerminal
  0x04,        // bNrChannels 4
  0x33, 0x00,  // wChannelConfig (Left and Right Front,Left and Right Surround)
  0x00,        // iChannelNames
  0x00,        // iTerminal

  0x0C,        // bLength
  0x24,        // bDescriptorType (See Next Line)
  0x06,        // bDescriptorSubtype (CS_INTERFACE -> FEATURE_UNIT)
  0x02,        // bUnitID
  0x01,        // bSourceID
  0x01,        // bControlSize 1
  0x03, 0x00,  // bmaControls[0] (Mute,Volume)
  0x00, 0x00,  // bmaControls[1] (None)
  0x00, 0x00,  // bmaControls[2] (None)

  0x09,        // bLength
  0x24,        // bDescriptorType (See Next Line)
  0x03,        // bDescriptorSubtype (CS_INTERFACE -> OUTPUT_TERMINAL)
  0x03,        // bTerminalID
  0x01, 0x03,  // wTerminalType (Speaker)
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
  0x04,        // bNrChannels 4
  0x02,        // bSubFrameSize 2
  0x10,        // bBitResolution 16
  0x01,        // bSamFreqType 1
  0x80, 0xBB, 0x00,  // tSamFreq[1] 48000 Hz

  0x09,        // bLength
  0x05,        // bDescriptorType (See Next Line)
  0x01,        // bEndpointAddress (OUT/H2D)
  0x09,        // bmAttributes (Isochronous, Adaptive, Data EP)
  0x88, 0x01,  // wMaxPacketSize 392
  0x01,        // bInterval 1 (unit depends on device speed)
  0x00,        // bRefresh
  0x00,        // bSyncAddress

  0x07,        // bLength
  0x25,        // bDescriptorType (See Next Line)
  0x01,        // bDescriptorSubtype (CS_ENDPOINT -> EP_GENERAL)
  0x01,        // bmAttributes (Sampling Freq Control)
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
  0x02,        // bNrChannels (Stereo)
  0x02,        // bSubFrameSize 2
  0x10,        // bBitResolution 16
  0x01,        // bSamFreqType 1
  0x80, 0xBB, 0x00,  // tSamFreq[1] 48000 Hz

  0x09,        // bLength
  0x05,        // bDescriptorType (See Next Line)
  0x82,        // bEndpointAddress (IN/D2H)
  0x05,        // bmAttributes (Isochronous, Async, Data EP)
  0xC4, 0x00,  // wMaxPacketSize 196
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
  0x11, 0x01,  // wDescriptorLength[0] 273

  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x84,        // bEndpointAddress (IN/D2H)
  0x03,        // bmAttributes (Interrupt)
  0x40, 0x00,  // wMaxPacketSize 64
  0x04,        // bInterval 4 (unit depends on device speed)

  0x07,        // bLength
  0x05,        // bDescriptorType (Endpoint)
  0x03,        // bEndpointAddress (OUT/H2D)
  0x03,        // bmAttributes (Interrupt)
  0x40, 0x00,  // wMaxPacketSize 64
  0x04,        // bInterval 4 (unit depends on device speed)

This is basically the same as the Dualshock, with twice as much audio channels.

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
  0x09, 0x33,        //   Usage (Rx)
  0x09, 0x34,        //   Usage (Ry)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x06,        //   Report Count (6)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x20,        //   Usage (0x20)
  0x95, 0x01,        //   Report Count (1)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x05, 0x01,        //   Usage Page (Generic Desktop Ctrls)
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
  0x29, 0x0F,        //   Usage Maximum (0x0F)
  0x15, 0x00,        //   Logical Minimum (0)
  0x25, 0x01,        //   Logical Maximum (1)
  0x75, 0x01,        //   Report Size (1)
  0x95, 0x0F,        //   Report Count (15)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x21,        //   Usage (0x21)
  0x95, 0x0D,        //   Report Count (13)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x06, 0x00, 0xFF,  //   Usage Page (Vendor Defined 0xFF00)
  0x09, 0x22,        //   Usage (0x22)
  0x15, 0x00,        //   Logical Minimum (0)
  0x26, 0xFF, 0x00,  //   Logical Maximum (255)
  0x75, 0x08,        //   Report Size (8)
  0x95, 0x34,        //   Report Count (52)
  0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
  0x85, 0x02,        //   Report ID (2)
  0x09, 0x23,        //   Usage (0x23)
  0x95, 0x2F,        //   Report Count (47)
  0x91, 0x02,        //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x05,        //   Report ID (5)
  0x09, 0x33,        //   Usage (0x33)
  0x95, 0x28,        //   Report Count (40)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x08,        //   Report ID (8)
  0x09, 0x34,        //   Usage (0x34)
  0x95, 0x2F,        //   Report Count (47)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x09,        //   Report ID (9)
  0x09, 0x24,        //   Usage (0x24)
  0x95, 0x13,        //   Report Count (19)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x0A,        //   Report ID (10)
  0x09, 0x25,        //   Usage (0x25)
  0x95, 0x1A,        //   Report Count (26)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x20,        //   Report ID (32)
  0x09, 0x26,        //   Usage (0x26)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x21,        //   Report ID (33)
  0x09, 0x27,        //   Usage (0x27)
  0x95, 0x04,        //   Report Count (4)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x22,        //   Report ID (34)
  0x09, 0x40,        //   Usage (0x40)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x80,        //   Report ID (-128)
  0x09, 0x28,        //   Usage (0x28)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x81,        //   Report ID (-127)
  0x09, 0x29,        //   Usage (0x29)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x82,        //   Report ID (-126)
  0x09, 0x2A,        //   Usage (0x2A)
  0x95, 0x09,        //   Report Count (9)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x83,        //   Report ID (-125)
  0x09, 0x2B,        //   Usage (0x2B)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x84,        //   Report ID (-124)
  0x09, 0x2C,        //   Usage (0x2C)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0x85,        //   Report ID (-123)
  0x09, 0x2D,        //   Usage (0x2D)
  0x95, 0x02,        //   Report Count (2)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xA0,        //   Report ID (-96)
  0x09, 0x2E,        //   Usage (0x2E)
  0x95, 0x01,        //   Report Count (1)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xE0,        //   Report ID (-32)
  0x09, 0x2F,        //   Usage (0x2F)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF0,        //   Report ID (-16)
  0x09, 0x30,        //   Usage (0x30)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF1,        //   Report ID (-15)
  0x09, 0x31,        //   Usage (0x31)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF2,        //   Report ID (-14)
  0x09, 0x32,        //   Usage (0x32)
  0x95, 0x0F,        //   Report Count (15)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF4,        //   Report ID (-12)
  0x09, 0x35,        //   Usage (0x35)
  0x95, 0x3F,        //   Report Count (63)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0x85, 0xF5,        //   Report ID (-11)
  0x09, 0x36,        //   Usage (0x36)
  0x95, 0x03,        //   Report Count (3)
  0xB1, 0x02,        //   Feature (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
  0xC0,              // End Collection

Input report structure
----------------------

The report ID is 0x01 over USB. Over Bluetooth, report 0x01 is a simplified version, as for the Dualshock, but after GET_REPORT 0x20 it becomes 0x31 which is similar to this in structure, but the report ID is followed by a single byte (0x51), so offsets have to be adjusted.

+--------+--------+--------------------------------------------------------------------------+
+ Offset + Type   + Meaning                                                                  +
+========+========+==========================================================================+
+ 0      + uint8  + Report ID                                                                +
+--------+--------+--------------------------------------------------------------------------+
+ 1      + uint8  + Left pad X (0x00 is left, 0xFF right)                                    +
+--------+--------+--------------------------------------------------------------------------+
+ 2      + uint8  + Left pad Y (0x00 is up, 0xFF down)                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 3      + uint8  + Right pad X                                                              +
+--------+--------+--------------------------------------------------------------------------+
+ 4      + uint8  + Right pad Y                                                              +
+--------+--------+--------------------------------------------------------------------------+
+ 5      + uint8  + L2 value                                                                 +
+--------+--------+--------------------------------------------------------------------------+
+ 6      + uint8  + R2 value                                                                 +
+--------+--------+--------------------------------------------------------------------------+
+ 7      + uint8  + A counter. This is incremented for each report.                          +
+--------+--------+--------------------------------------------------------------------------+
+ 8      + uint4  + DPad                                                                     +
+--------+--------+--------------------------------------------------------------------------+
+ 8      + bool   + Square                                                                   +
+--------+--------+--------------------------------------------------------------------------+
+ 8      + bool   + Cross                                                                    +
+--------+--------+--------------------------------------------------------------------------+
+ 8      + bool   + Circle                                                                   +
+--------+--------+--------------------------------------------------------------------------+
+ 8      + bool   + Triangle                                                                 +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + R3                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + L3                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + Options                                                                  +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + Share                                                                    +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + L1                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + R1                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + L2                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 9      + bool   + R2                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 10     + uint5  + Unknown                                                                  +
+--------+--------+--------------------------------------------------------------------------+
+ 10     + bool   + Mute                                                                     +
+--------+--------+--------------------------------------------------------------------------+
+ 10     + bool   + TPad                                                                     +
+--------+--------+--------------------------------------------------------------------------+
+ 10     + bool   + PS                                                                       +
+--------+--------+--------------------------------------------------------------------------+
+ 11     + uint8  + Unknown                                                                  +
+--------+--------+--------------------------------------------------------------------------+
+ 12-15  + uint32 + Another counter, on 32 bits. Not sure about the highest byte.            +
+--------+--------+--------------------------------------------------------------------------+
+ 16-29  +        + Probably IMU values, yet to confirm                                      +
+--------+--------+--------------------------------------------------------------------------+
+ 30     + uint16 + Timestamp (unit is probably 5.33ms)                                      +
+--------+--------+--------------------------------------------------------------------------+
+ 32     + uint8  + Unknown, probably battery                                                +
+--------+--------+--------------------------------------------------------------------------+
+ 33-63  +        + Unknown, probably touchpad (among other stuff)                           +
+--------+--------+--------------------------------------------------------------------------+

Known feature reports
---------------------

GET_REPORT 0x05
###############

IMU calibration data. Details still to be investigated.

GET_REPORT 0x20
###############

Manufacturing info; this is the equivalent of the Dualshock's 0xa3 report.

GET_REPORT 0x09
###############

Exact same structure as the Dualshock's 0x12 report (pairing state).

SET_REPORT 0x0a
###############

Exact same structure as the Dualshock's 0x13 report (set pairing and link key).

SET_REPORT 0x08
###############

Exact same structure as the Dualshock's 0x14 report (connect/disconnect).
