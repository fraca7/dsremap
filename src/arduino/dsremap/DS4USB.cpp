
#include "DS4USB.h"
#include "DS4Structs.h"

#include "Host.h"

DS4USB::DS4USB(USB* pUsb, Host* pHost)
  : USBController(pUsb, pHost)
{
}

void DS4USB::GetCalibrationData()
{
  GET_REPORT(0x03, 0x02, 37);
}

void DS4USB::ControllerStateFromBuffer(controller_state_t* state, const uint8_t* data)
{
  memcpy((uint8_t*)state, data + 1, sizeof(*state));
}

void DS4USB::ControllerStateToBuffer(const controller_state_t* state, uint8_t* data)
{
  memcpy(data + 1, (uint8_t*)state, sizeof(*state));
}

void DS4USB::IMUStateFromBuffer(imu_state_t* state, const uint8_t* data)
{
  memcpy((uint8_t*)state, data + 10, sizeof(*state));
}

void DS4USB::IMUStateToBuffer(const imu_state_t* state, uint8_t* data)
{
  memcpy(data + 10, (const uint8_t*)state, sizeof(*state));
}
