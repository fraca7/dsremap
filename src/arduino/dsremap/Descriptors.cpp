
#include "Config.h"
#include "Descriptors.h"
#include "Host.h"

#include "messages.h"

// C++ compat...
#undef USB_STRING_DESCRIPTOR
#define USB_STRING_DESCRIPTOR(s) { .Header = { .Size = sizeof(USB_Descriptor_Header_t) + sizeof(s) - 2, .Type = DTYPE_String }, s }

static const USB_Descriptor_String_t PROGMEM LanguageString = USB_STRING_DESCRIPTOR_ARRAY(LANGUAGE_ID_ENG);
static const USB_Descriptor_String_t PROGMEM ManufacturerString = USB_STRING_DESCRIPTOR(L"Sony Interactive Entertainment");
static const USB_Descriptor_String_t PROGMEM ProductString = USB_STRING_DESCRIPTOR(L"Wireless Controller");

static const uint8_t SoundInterfaces[184] PROGMEM = {
  // As mentioned in the .h we don't need to handle the audio classes but we need to declare them
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
  0x00, 0x00   // wLockDelay 0
};

static const USB_Descriptor_Device_t PROGMEM DeviceDescriptor = {
  .Header = { .Size = sizeof(USB_Descriptor_Device_t), .Type = DTYPE_Device },

  .USBSpecification = VERSION_BCD(2, 0, 0),
  .Class = USB_CSCP_NoDeviceClass,
  .SubClass = USB_CSCP_NoDeviceSubclass,
  .Protocol = USB_CSCP_NoDeviceProtocol,
  .Endpoint0Size = DEVICE_EP_SIZE,
  .VendorID = 0x054c,
  .ProductID = 0x09cc,
  .ReleaseNumber = VERSION_BCD(1, 0, 0),
  .ManufacturerStrIndex = STRING_ID_Manufacturer,
  .ProductStrIndex = STRING_ID_Product,
  .SerialNumStrIndex = NO_DESCRIPTOR,
  .NumberOfConfigurations = 1
};

static const USB_Descriptor_Configuration_t PROGMEM ConfigurationDescriptor = {
  .Config = {
    .Header = { .Size = sizeof(USB_Descriptor_Configuration_Header_t), .Type = DTYPE_Configuration },
    .TotalConfigurationSize = sizeof(USB_Descriptor_Configuration_t),
    .TotalInterfaces = 1,
    .ConfigurationNumber = 1,
    .ConfigurationStrIndex = 0,
    .ConfigAttributes = USB_CONFIG_ATTR_RESERVED | USB_CONFIG_ATTR_SELFPOWERED,
    .MaxPowerConsumption = USB_CONFIG_POWER_MA(500)
  },

  .HID_Interface = {
    .Header = { .Size = sizeof(USB_Descriptor_Interface_t), .Type = DTYPE_Interface },
    .InterfaceNumber = DEVICE_INTERFACE_NUMBER,
    .AlternateSetting = 0,
    .TotalEndpoints = 2,
    .Class = HID_CSCP_HIDClass,
    .SubClass = HID_CSCP_NonBootSubclass,
    .Protocol = HID_CSCP_NonBootProtocol,
    .InterfaceStrIndex = 0
  },

  .HID = {
    .Header = { .Size = sizeof(USB_HID_Descriptor_HID_t), .Type = HID_DTYPE_HID },
    .HIDSpec = VERSION_BCD(1, 1, 1),
    .CountryCode = 0,
    .TotalReportDescriptors = 1,
    .HIDReportType = HID_DTYPE_Report,
    .HIDReportLength = sizeof(usbHidReportDescriptor)
  },

  .HID_ReportINEndpoint = {
    .Header = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
    .EndpointAddress = DEVICE_EPADDR_IN,
    .Attributes = EP_TYPE_INTERRUPT|ENDPOINT_ATTR_NO_SYNC|ENDPOINT_USAGE_DATA,
    .EndpointSize = DEVICE_EP_SIZE,
    .PollingIntervalMS = 0x05
  },

  .HID_ReportOUTEndpoint = {
    .Header = { .Size = sizeof(USB_Descriptor_Endpoint_t), .Type = DTYPE_Endpoint },
    .EndpointAddress = DEVICE_EPADDR_OUT,
    .Attributes = EP_TYPE_INTERRUPT|ENDPOINT_ATTR_NO_SYNC|ENDPOINT_USAGE_DATA,
    .EndpointSize = DEVICE_EP_SIZE,
    .PollingIntervalMS = 0x05
  }
};

ClassInfo_t GlobalInterface;

//==============================================================================
// Events

void EVENT_USB_Device_Connect()
{
  LogInfo(USB_CONN);
}

void EVENT_USB_Device_Disconnect()
{
  LogInfo(USB_DISCONN);
}

void EVENT_USB_Device_ConfigurationChanged()
{
  LogDebug(USB_CONF_CHANGED);

  memset(&GlobalInterface.USB.State, 0x00, sizeof(GlobalInterface.USB.State));
  GlobalInterface.USB.State.UsingReportProtocol = true;
  GlobalInterface.USB.State.IdleCount = 0;

  Endpoint_ConfigureEndpoint(DEVICE_EPADDR_IN, EP_TYPE_INTERRUPT, DEVICE_EP_SIZE, 1);
  Endpoint_ConfigureEndpoint(DEVICE_EPADDR_OUT, EP_TYPE_INTERRUPT, DEVICE_EP_SIZE, 1);

  USB_Device_EnableSOFEvents();
}

void EVENT_USB_Device_ControlRequest()
{
  if (!Endpoint_IsSETUPReceived())
    return;

  if (((USB_ControlRequest.bmRequestType & CONTROL_REQTYPE_RECIPIENT) == REQREC_INTERFACE) && ((USB_ControlRequest.wIndex & 0xFF) < DEVICE_INTERFACE_NUMBER)) {
    switch (USB_ControlRequest.bRequest) {
      case AUDIO_REQ_GetMinimum:
        if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST|REQTYPE_CLASS|REQREC_INTERFACE) {
          uint8_t minval[2];
          switch (USB_ControlRequest.wIndex >> 8) {
            case 0x02:
              minval[0] = 0x00;
              minval[1] = 0xb7;
              break;
            case 0x05:
              minval[0] = 0xc0;
              minval[1] = 0xe8;
              break;
            default:
              LogWarning(WARN_MIN_UNK_ENTITY, USB_ControlRequest.wIndex >> 8);
              break;
          }

          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(minval, 2);
          Endpoint_ClearOUT();
          return;
        }
        break;
      case AUDIO_REQ_GetMaximum:
        if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST|REQTYPE_CLASS|REQREC_INTERFACE) {
          uint8_t maxval[2];
          switch (USB_ControlRequest.wIndex >> 8) {
            case 0x02:
              maxval[0] = 0x00;
              maxval[1] = 0xff;
              break;
            case 0x05:
              maxval[0] = 0x00;
              maxval[1] = 0x18;
              break;
            default:
              LogWarning(WARN_MAX_UNK_ENTITY, USB_ControlRequest.wIndex >> 8);
              break;
          }

          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(maxval, 2);
          Endpoint_ClearOUT();
          return;
        }
        break;
      case AUDIO_REQ_GetResolution:
        if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST|REQTYPE_CLASS|REQREC_INTERFACE) {
          uint8_t resval[2];
          switch (USB_ControlRequest.wIndex >> 8) {
            case 0x02:
              resval[0] = 0x00;
              resval[1] = 0x01;
              break;
            case 0x05:
              resval[0] = 0xc0;
              resval[1] = 0x00;
              break;
            default:
              LogWarning(WARN_RES_UNK_ENTITY, USB_ControlRequest.wIndex >> 8);
              break;
          }

          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(resval, 2);
          Endpoint_ClearOUT();
          return;
        }
        break;
      case AUDIO_REQ_SetCurrent:
        if (USB_ControlRequest.bmRequestType == REQDIR_HOSTTODEVICE|REQTYPE_CLASS|REQREC_INTERFACE) {
          uint8_t current[2];
          Endpoint_ClearSETUP();
          Endpoint_Read_Control_Stream_LE(current, 2);
          Endpoint_ClearIN();
          return;
        }
        break;
      case AUDIO_REQ_GetCurrent:
        if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST|REQTYPE_CLASS|REQREC_INTERFACE) {
          uint8_t current[2] = {0x00, 0x00};
          Endpoint_ClearSETUP();
          Endpoint_Write_Control_Stream_LE(current, 2);
          Endpoint_ClearOUT();
          return;
        }
      case REQ_SetInterface:
        Endpoint_ClearSETUP();
        Endpoint_ClearStatusStage();
        break;
      default:
        LogWarning(WARN_UNHANDLED_REQ);
        LogDump(sizeof(USB_ControlRequest), (uint8_t*)&USB_ControlRequest);
        break;
    }
  }

  if (USB_ControlRequest.wIndex != ConfigurationDescriptor.HID_Interface.InterfaceNumber)
    return;

  switch (USB_ControlRequest.bRequest) {
    case HID_REQ_GetReport:
      if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE) {
        uint16_t ReportSize = USB_ControlRequest.wLength;
        uint8_t  ReportID   = (USB_ControlRequest.wValue & 0xFF);
        uint8_t  ReportType = (USB_ControlRequest.wValue >> 8) - 1;

        GlobalInterface.pHost->CreateHIDReport(ReportID, ReportType, ReportSize);
        GlobalInterface.USB.State.IdleMSRemaining = GlobalInterface.USB.State.IdleCount;
      }
      break;
    case HID_REQ_SetReport:
      if (USB_ControlRequest.bmRequestType == REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE) {
        uint16_t ReportSize = USB_ControlRequest.wLength;
        uint8_t  ReportID = (USB_ControlRequest.wValue & 0xFF);
        uint8_t  ReportType = (USB_ControlRequest.wValue >> 8) - 1;
        uint8_t  ReportData[ReportSize];

        Endpoint_ClearSETUP();
        Endpoint_Read_Control_Stream_LE(ReportData, ReportSize);
        Endpoint_ClearIN();

        GlobalInterface.pHost->ProcessHIDReport(ReportID, ReportType, ReportData, ReportSize);
      }
      break;
    case HID_REQ_GetIdle:
      if (USB_ControlRequest.bmRequestType == REQDIR_DEVICETOHOST | REQTYPE_CLASS | REQREC_INTERFACE) {
        Endpoint_ClearSETUP();
        while (!(Endpoint_IsINReady()));
        Endpoint_Write_8(GlobalInterface.USB.State.IdleCount >> 2);
        Endpoint_ClearIN();
        Endpoint_ClearStatusStage();
      }
      break;
    case HID_REQ_SetIdle:
      if (USB_ControlRequest.bmRequestType == REQDIR_HOSTTODEVICE | REQTYPE_CLASS | REQREC_INTERFACE) {
        Endpoint_ClearSETUP();
        Endpoint_ClearStatusStage();

        GlobalInterface.USB.State.IdleCount = ((USB_ControlRequest.wValue & 0xFF00) >> 6);
      }
      break;
  }
}

void EVENT_USB_Device_StartOfFrame()
{
  if (GlobalInterface.USB.State.IdleMSRemaining)
    GlobalInterface.USB.State.IdleMSRemaining--;
}

uint16_t CALLBACK_USB_GetDescriptor(const uint16_t wValue, const uint16_t wIndex, const void** const DescriptorAddress)
{
  const uint8_t descType = wValue >> 8;
  const uint8_t descNumber = wValue & 0xFF;

  const void* buf = NULL;
  uint16_t len = 0;

  switch (descType) {
    case DTYPE_Device:
      LogDebug(SEND_DEV_DES);
      buf = &DeviceDescriptor;
      len = sizeof(USB_Descriptor_Device_t);
      break;
    case DTYPE_Configuration:
    {
      LogDebug(SEND_CONF_DES);

      if (GlobalInterface.pHost->ShouldSpoofAudio()) {
        uint8_t data[sizeof(ConfigurationDescriptor) + sizeof(SoundInterfaces)];
        memcpy_P(data, &ConfigurationDescriptor.Config, sizeof(ConfigurationDescriptor.Config));
        memcpy_P(data + sizeof(ConfigurationDescriptor.Config), SoundInterfaces, sizeof(SoundInterfaces));
        memcpy_P(data + sizeof(ConfigurationDescriptor.Config) + sizeof(SoundInterfaces), &ConfigurationDescriptor.HID_Interface, sizeof(ConfigurationDescriptor) - sizeof(ConfigurationDescriptor.Config));
        ((USB_Descriptor_Configuration_Header_t*)data)->TotalConfigurationSize += sizeof(SoundInterfaces);
        ((USB_Descriptor_Configuration_Header_t*)data)->TotalInterfaces = 3;

        Endpoint_ClearSETUP();
        Endpoint_Write_Control_Stream_LE(data, sizeof(data));
        Endpoint_ClearOUT();

        len = NO_DESCRIPTOR;
      } else {
        buf = &ConfigurationDescriptor;
        len = sizeof(ConfigurationDescriptor);
      }

      break;
    }
    case HID_DTYPE_HID:
      LogDebug(SEND_HID_DES);
      buf = &ConfigurationDescriptor.HID;
      len = sizeof(USB_HID_Descriptor_HID_t);
      break;
    case HID_DTYPE_Report:
      LogDebug(SEND_HID_REP_DES);
      buf = usbHidReportDescriptor;
      len = sizeof(usbHidReportDescriptor);
      GlobalInterface.pHost->ReportDescriptorSent();
      break;
    case DTYPE_String:
      switch (descNumber)
      {
        case STRING_ID_Language:
          buf = &LanguageString;
          len = pgm_read_byte(&LanguageString.Header.Size);
          break;
        case STRING_ID_Manufacturer:
          buf = &ManufacturerString;
          len = pgm_read_byte(&ManufacturerString.Header.Size);
          break;
        case STRING_ID_Product:
          buf = &ProductString;
          len = pgm_read_byte(&ProductString.Header.Size);
          break;
      }
      break;
  }

  *DescriptorAddress = buf;
  return len;
}
