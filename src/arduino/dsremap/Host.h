
#ifndef _HOST_H
#define _HOST_H

#include "Config.h"
#include "LUFAConfig.h"

#include "Descriptors.h"
#include "CommonStructs.h"

#ifndef DISABLE_REMAPPING
#include "Configuration.h"
#include "blink.h"
#endif
#if defined(INVERT_GYRO_X) || defined(INVERT_GYRO_Y) || defined(INVERT_GYRO_Z) || !defined(DISABLE_REMAPPING)
#include "IMUIntegrator.h"
#endif

class USBController;

#ifndef DISABLE_REMAPPING
class BytecodeWriter;
#endif

/*
 * Class that interfaces with the PS4
 */
class Host
{
public:
  Host();

  void OnDeviceReady(USBController*);
  void OnDeviceData(uint16_t, uint8_t*);
  void SendReport(uint16_t, const uint8_t*, USB_DescriptorMemorySpaces_t = MEMSPACE_RAM);
  void SetCalibrationData(CalibrationData_t*);

  // LUFA callbacks
  void ReportDescriptorSent();
  void CreateHIDReport(uint8_t reportId, const uint8_t reportType, uint16_t len);
  void ProcessHIDReport(const uint8_t reportId, const uint8_t reportType, const uint8_t* data, const uint16_t len);
  void GotDataFromHost(const uint8_t* buf, uint16_t len);
  bool ShouldSpoofAudio() const;

  void loop();

private:
  USBController* m_pDevice;
#if defined(INVERT_GYRO_X) || defined(INVERT_GYRO_Y) || defined(INVERT_GYRO_Z) || !defined(DISABLE_REMAPPING)
  IMUIntegrator m_Integrator;
#endif
  uint8_t m_OutReport[DEVICE_EP_SIZE];
  uint8_t m_OutLen;
  uint8_t m_ErrorCount;

#ifndef DISABLE_REMAPPING
  DArray<Configuration> m_Configurations;
  uint8_t m_CurrentConfig;
  ButtonState m_HatState;
  Blink m_Blink;

  BytecodeWriter* m_pWriter;
#endif

  enum class HostState {
    PC_Config,
    PS4_Config,
#ifndef DISABLE_REMAPPING
    ReceivingBytecode,
#endif
    PC_Enumerating,
    PC,
    PS4_Enumerating,
    PS4
  };
  HostState m_HostState;

#ifndef DISABLE_REMAPPING
  void InstantiateVMs();
#endif
};

#endif /* _HOST_H */
