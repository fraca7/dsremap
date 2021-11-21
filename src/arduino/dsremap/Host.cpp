
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <EEPROM.h>

#include "Host.h"
#include "Descriptors.h"
#include "BytecodeWriter.h"
#include "USBController.h"
#include "VM.h"
#include "version.h"

#define SET_STATE(state) { LogDebug(STATE_CHANGE, static_cast<uint8_t>(m_HostState), static_cast<uint8_t>(state)); m_HostState = state; }

Host::Host()
  : m_pDevice(NULL),
#ifndef DISABLE_REMAPPING
    m_Integrator(),
#endif
    m_OutLen(0),
    m_ErrorCount(0),
#ifndef DISABLE_REMAPPING
    m_Configurations(),
    m_CurrentConfig(0),
    m_HatState(ButtonState::Released),
    m_Blink(),
    m_pWriter(NULL),
#endif
    m_HostState(HostState::PC_Enumerating)
{
  /* Disable watchdog if enabled by bootloader/fuses */
  MCUSR &= ~(1 << WDRF);
  wdt_disable();

  /* Disable clock division */
  clock_prescale_set(clock_div_1);

  GlobalInterruptEnable();

  memset(&GlobalInterface, 0, sizeof(GlobalInterface));
  GlobalInterface.USB.Config.InterfaceNumber = DEVICE_INTERFACE_NUMBER;
  GlobalInterface.USB.Config.ReportINEndpoint.Address = DEVICE_EPADDR_IN;
  GlobalInterface.USB.Config.ReportINEndpoint.Size = DEVICE_EP_SIZE;
  GlobalInterface.USB.Config.ReportINEndpoint.Banks = 1;
  GlobalInterface.USB.Config.PrevReportINBufferSize = DEVICE_EP_SIZE;
  GlobalInterface.pHost = this;

  USB_Init();
}

void Host::OnDeviceReady(USBController* pDevice)
{
  m_pDevice = pDevice;
  USB_Device_CurrentlySelfPowered = true;

  switch (m_HostState) {
    case HostState::PC:
    case HostState::PC_Enumerating:
      SET_STATE(HostState::PC_Enumerating);
      USB_Init();
      break;
#ifndef DISABLE_REMAPPING
    case HostState::ReceivingBytecode:
#endif
    case HostState::PS4_Enumerating:
      // Should not happen
      break;
    case HostState::PC_Config:
    case HostState::PS4_Config:
    case HostState::PS4:
      LogError(ERROR_DEVICE_PLUG, static_cast<uint8_t>(m_HostState));
      break;
  }
}

void Host::OnDeviceData(uint16_t len, uint8_t* report)
{
  switch (m_HostState) {
    case HostState::PC_Enumerating:
    case HostState::PS4_Enumerating:
      return;
    case HostState::PC_Config:
    case HostState::PS4_Config:
#ifndef DISABLE_REMAPPING
    case HostState::ReceivingBytecode:
#endif
    case HostState::PC:
    case HostState::PS4:
      break;
  }

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
  static unsigned long last = millis();
  static uint16_t counter = 1;

  unsigned long now = millis();
  if (now - last > 1000) {
    LogTrace(REPORT_COUNT, counter, now - last);
    last = now;
    counter = 1;
  } else {
    ++counter;
  }
#endif

  if (m_OutLen)
    LogWarning(LOST_REPORT);

  memcpy(m_OutReport, report, len);
  m_OutLen = len;
}

void Host::SetCalibrationData(CalibrationData_t* pData)
{
  LogInfo(GOT_CALIB);
  LogDump(sizeof(*pData), (uint8_t*)pData);

#ifndef DISABLE_REMAPPING
  m_Integrator.SetCalibrationData(pData);
#endif
}

void Host::SendReport(uint16_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace)
{
  switch (m_HostState) {
    case HostState::PC_Enumerating:
    case HostState::PS4_Enumerating:
      LogWarning(WARN_OOB_REPORT_DATA, buf[0], static_cast<uint8_t>(m_HostState));
      LogDump(len, buf);
      return;
    case HostState::PC_Config:
    case HostState::PS4_Config:
#ifndef DISABLE_REMAPPING
    case HostState::ReceivingBytecode:
#endif
    case HostState::PC:
    case HostState::PS4:
      break;
  }

  switch (memspace) {
    case MEMSPACE_RAM:
      LogDebug(GOT_REPORT_DATA_R);
      break;
    case MEMSPACE_FLASH:
      LogDebug(GOT_REPORT_DATA_F);
      break;
    default:
      LogError(ERROR_UNSUPPORTED_MEMSPACE);
      return;
  }

  LogDump(len, buf, memspace);

  uint8_t ep = Endpoint_GetCurrentEndpoint();
  Endpoint_SelectEndpoint(ENDPOINT_CONTROLEP);
  Endpoint_ClearSETUP();
  switch (memspace) {
    case MEMSPACE_RAM:
      Endpoint_Write_Control_Stream_LE(buf, len);
      break;
    case MEMSPACE_FLASH:
      Endpoint_Write_Control_PStream_LE(buf, len);
      break;
    default:
      break;
  }
  Endpoint_ClearOUT();
  Endpoint_SelectEndpoint(ep);
}

void Host::loop()
{
#if LOG_LEVEL >= LOG_LEVEL_DEBUG
  static unsigned long last = millis();
  static uint16_t counter = 1;

  unsigned long now = millis();
  if (now - last > 1000) {
    LogTrace(LOOP_COUNT, counter, now - last);
    last = now;
    counter = 1;
  } else {
    ++counter;
  }
#endif

  switch (m_HostState) {
    case HostState::PC_Config:
    case HostState::PS4_Config:
    case HostState::PC:
    case HostState::PS4:
      if (m_OutLen) {
        controller_state_t ctrl;
        m_pDevice->ControllerStateFromBuffer(&ctrl, m_OutReport);

#ifndef DISABLE_REMAPPING
        imu_state_t imu;
        m_pDevice->IMUStateFromBuffer(&imu, m_OutReport);

        m_Integrator.Update(&imu);

        if (m_CurrentConfig < m_Configurations.Count())
          m_Configurations.GetItem(m_CurrentConfig)->Run(&ctrl, &m_Integrator);

        m_Blink.loop();

        if (ctrl.PS) {
          if (m_HostState == HostState::PC_Config) {
            m_Blink.SetBlinkCount(0);
            SET_STATE(HostState::PC);
          } else if (m_HostState == HostState::PS4_Config) {
            m_Blink.SetBlinkCount(0);
            SET_STATE(HostState::PS4);
          }
        }

        if ((m_HostState == HostState::PC_Config) || (m_HostState == HostState::PS4_Config)) {
          switch (m_HatState) {
            case ButtonState::Released:
              if (ctrl.Hat == 0x00)
                m_HatState = ButtonState::Pressed;
              break;
            case ButtonState::Pressed:
              if (ctrl.Hat != 0x00) {
                m_HatState = ButtonState::Released;
                m_CurrentConfig = (m_CurrentConfig + 1) % m_Configurations.Count();
                m_Blink.SetBlinkCount(m_CurrentConfig + 1);
              }
              break;
          }
        }

        m_pDevice->ControllerStateToBuffer(&ctrl, m_OutReport);
#endif

        Endpoint_SelectEndpoint(DEVICE_EPADDR_IN);
        uint8_t rcode = Endpoint_Write_Stream_LE(m_OutReport, m_OutLen, NULL);
        Endpoint_ClearIN();
        m_OutLen = 0;

        if (rcode != ENDPOINT_RWSTREAM_NoError) {
          LogError(ERROR_SEND_REPORT_01, rcode);

          if ((rcode == ENDPOINT_RWSTREAM_Timeout) && ((m_HostState == HostState::PC) || (m_HostState == HostState::PC_Config))) {
#ifndef DISABLE_REMAPPING
            m_Blink.SetBlinkCount(0);
#endif

            // Heuristic to find out we're connected to a PS4 instead of a PC...
            if (m_pDevice) { // Only go to PS4_Enumerating if the controller is plugged.
              ++m_ErrorCount;
              if (m_ErrorCount >= 16) {
                SET_STATE(HostState::PS4_Enumerating);
                USB_Init();
              }
            }
          }
        }
      }
#ifndef DISABLE_REMAPPING
      // NO BREAK
    case HostState::ReceivingBytecode:
      // LUFA's HID class system does not support an OUT endpoint, do it by hand.
      Endpoint_SelectEndpoint(DEVICE_EPADDR_OUT);

      if (Endpoint_IsOUTReceived()) {
        uint16_t count = Endpoint_BytesInEndpoint();
        if (count) {
          uint8_t buf[DEVICE_EP_SIZE];
          uint8_t rcode = Endpoint_Read_Stream_LE(buf, count, NULL);
          if (rcode != ENDPOINT_RWSTREAM_NoError) {
            LogError(ERROR_READ_FROM_HOST, rcode);
          } else {
            LogDebug(RECV_FROM_HOST, count);
            LogDump(count, buf);

            GotDataFromHost(buf, count);
          }

          Endpoint_ClearOUT();
        }
      }
#endif
      // NO BREAK
    case HostState::PC_Enumerating:
    case HostState::PS4_Enumerating:
      USB_USBTask();
      break;
  }
}

void Host::ReportDescriptorSent()
{
  switch (m_HostState) {
    case HostState::PC_Enumerating:
#ifdef DISABLE_REMAPPING
      SET_STATE(HostState::PC);
#else
      if (m_pDevice) {
        InstantiateVMs();

        if (m_Configurations.Count() > 1) {
          SET_STATE(HostState::PC_Config);
          m_Blink.SetBlinkCount(m_CurrentConfig + 1);
        } else {
          SET_STATE(HostState::PC);
        }
      } else {
        SET_STATE(HostState::PC);
      }
#endif
      break;
    case HostState::PS4_Enumerating:
#ifdef DISABLE_REMAPPING
      SET_STATE(HostState::PS4);
#else
      InstantiateVMs();
      if (m_Configurations.Count() > 1) {
        SET_STATE(HostState::PS4_Config);
        m_Blink.SetBlinkCount(m_CurrentConfig + 1);
      } else {
        SET_STATE(HostState::PS4);
      }
#endif
      break;
    case HostState::PC_Config:
    case HostState::PS4_Config:
#ifndef DISABLE_REMAPPING
    case HostState::ReceivingBytecode:
#endif
    case HostState::PC:
    case HostState::PS4:
      LogError(ERROR_REPORTDESC, static_cast<uint8_t>(m_HostState));
      break;
  }
}

void Host::CreateHIDReport(uint8_t reportId, const uint8_t reportType, uint16_t len)
{
  LogDebug(INFO_GET_REPORT, reportType, reportId);

  switch (m_HostState) {
    case HostState::PC_Config:
    case HostState::PC:
#ifndef DISABLE_REMAPPING
      if (reportId == 0x80) {
        struct {
          uint8_t id;
          uint16_t major;
          uint16_t minor;
          uint16_t patch;
        } version;

        version.id = 0x80;
        version.major = FW_VERSION_MAJOR;
        version.minor = FW_VERSION_MINOR;
        version.patch = FW_VERSION_PATCH;
        SendReport(sizeof(version), (uint8_t*)&version, MEMSPACE_RAM);

        return;
      }
#endif
      break;
    case HostState::PS4_Config:
    case HostState::PS4:
      break;
#ifndef DISABLE_REMAPPING
    case HostState::ReceivingBytecode:
      if (reportId == 0x84) {
        uint8_t buf[5];
        buf[0] = 0x84;
        *((uint32_t*)(buf + 1)) = m_pWriter->GetChecksum();
        SendReport(sizeof(buf), buf, MEMSPACE_RAM);

        delete m_pWriter;
        m_pWriter = NULL;

        InstantiateVMs();

        if (m_Configurations.Count() > 1) {
          SET_STATE(HostState::PC_Config);
          m_Blink.SetBlinkCount(m_CurrentConfig + 1);
        } else {
          SET_STATE(HostState::PC);
        }
        return;
      }
      break;
#endif
    case HostState::PC_Enumerating:
    case HostState::PS4_Enumerating:
      LogError(ERROR_GET_REPORT_STATE, static_cast<uint8_t>(m_HostState));
      return;
  }

  if (m_pDevice)
    m_pDevice->GET_REPORT(reportType, reportId, len);
}

void Host::ProcessHIDReport(uint8_t reportId, uint8_t reportType, const uint8_t* data, uint16_t len)
{
  LogDebug(INFO_SET_REPORT, reportType, reportId, len);

  switch (m_HostState) {
    case HostState::PC:
    case HostState::PC_Config:
      if (m_pDevice && (reportId == 0x05)) {
        LogDebug(SEND_OUT_EP, len);
        LogDump(len, (uint8_t*)data);
        m_pDevice->SendData(len, (uint8_t*)data);
        return;
      }
#ifndef DISABLE_REMAPPING
      if (reportId == 0x88) {
        SET_STATE(HostState::ReceivingBytecode);
        m_pWriter = new BytecodeWriter();
        m_pWriter->OnData(data, len);
        return;
      }
#endif
      break;
    case HostState::PS4:
    case HostState::PS4_Config:
      break;
#ifndef DISABLE_REMAPPING
    case HostState::ReceivingBytecode:
      if (reportId == 0x88) {
        m_pWriter->OnData(data, len);
        return;
      }
      break;
#endif
    case HostState::PC_Enumerating:
    case HostState::PS4_Enumerating:
      LogError(ERROR_SET_REPORT_STATE, static_cast<uint8_t>(m_HostState));
      return;
  }

  LogDebug(SEND_SET_REPORT, len);
  LogDump(len, data);

  if (m_pDevice)
    m_pDevice->SET_REPORT(reportType, reportId, len, data);
}

void Host::GotDataFromHost(const uint8_t* buf, uint16_t len)
{
  LogDebug(GOT_DATA_FROM_HOST, len);
  LogDump(len, buf);

  switch (m_HostState) {
    case HostState::PC:
    case HostState::PC_Config:
    case HostState::PS4:
    case HostState::PS4_Config:
      break;
    case HostState::PC_Enumerating:
    case HostState::PS4_Enumerating:
      LogError(ERROR_GOT_DATA_STATE, static_cast<uint8_t>(m_HostState));
      return;
  }

  if (m_pDevice)
    m_pDevice->SendData(len, buf);
}

bool Host::ShouldSpoofAudio() const
{
  return m_HostState == HostState::PS4_Enumerating;
}

#ifndef DISABLE_REMAPPING

#define EEPROM_GET(x) do { EEPROM.get(offset, x); offset += sizeof(x); } while (false)

void Host::InstantiateVMs()
{
  uint16_t offset = 0;

  {
    uint16_t magic;
    EEPROM_GET(magic);

    if (magic != 0xCAFE) {
      LogError(ERROR_WRONG_MAGIC, magic);
      return;
    }
  }

  m_Configurations.Clear();
  m_CurrentConfig = 0;

  uint16_t conflen = 0;
  uint8_t count = 0;

  for (;;) {
    EEPROM_GET(conflen);
    if (!conflen) // Sentinel
      break;

    LogInfo(CONFIGURATION_SIZE, conflen);

    Configuration* pConfig = new Configuration();

    while (conflen) {
      uint16_t actionlen;
      EEPROM_GET(actionlen);

      LogInfo(ACTION_SIZE, actionlen);

      uint16_t stacksize;
      EEPROM_GET(stacksize);

      LogInfo(ACTION_STACKSIZE, stacksize);

      uint8_t* bytecode = (uint8_t*)malloc(actionlen - 2);
      for (uint16_t i = 0; i < actionlen - 2; ++i)
        bytecode[i] = EEPROM.read(offset++);

      pConfig->AddItem(new VM(bytecode, true, stacksize));
      ++count;

      conflen -= actionlen + sizeof(actionlen);
    }

    m_Configurations.AddItem(pConfig);
  };

  LogInfo(VM_DONE, count);
}

#endif
