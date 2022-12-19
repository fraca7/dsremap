
#include "DS5USB.h"
#include "DS5Structs.h"

#include "Host.h"

DS5USB::DS5USB(USB* pUsb, Host* pHost)
  : USBController(pUsb, pHost)
{
}

void DS5USB::GetCalibrationData()
{
  GET_REPORT(0x03, 0x05, 41);
}

void DS5USB::ControllerStateFromBuffer(controller_state_t* state, const uint8_t* data)
{
  const USBReport01_t* r= (const USBReport01_t*)data;

  state->LPadX = r->LPadX;
  state->RPadX = r->RPadX;
  state->LPadY = r->LPadY;
  state->RPadY = r->RPadY;
  state->L2Value = r->L2Value;
  state->R2Value = r->R2Value;
  state->Hat = r->Hat;
  state->Square = r->Square;
  state->Cross = r->Cross;
  state->Circle = r->Circle;
  state->Triangle = r->Triangle;
  state->R3 = r->R3;
  state->L3 = r->L3;
  state->Options = r->Options;
  state->Share = r->Share;
  state->L1 = r->L1;
  state->R1 = r->R1;
  state->L2 = r->L2;
  state->R2 = r->R2;
  state->PS = r->PS;
}

void DS5USB::ControllerStateToBuffer(const controller_state_t* state, uint8_t* data)
{
  USBReport01_t* r = (USBReport01_t*)data;

  // This does not work. The input reports include a 64-bits
  // "checksum" (more probably digital signature) and the console
  // ignores reports that have been tampered with.

  r->LPadX = state->LPadX;
  r->RPadX = state->RPadX;
  r->LPadY = state->LPadY;
  r->RPadY = state->RPadY;
  r->L2Value = state->L2Value;
  r->R2Value = state->R2Value;
  r->Hat = state->Hat;
  r->Square = state->Square;
  r->Cross = state->Cross;
  r->Circle = state->Circle;
  r->Triangle = state->Triangle;
  r->R3 = state->R3;
  r->L3 = state->L3;
  r->Options = state->Options;
  r->Share = state->Share;
  r->L1 = state->L1;
  r->R1 = state->R1;
  r->L2 = state->L2;
  r->R2 = state->R2;
  r->PS = state->PS;
}

void DS5USB::IMUStateFromBuffer(imu_state_t* state, const uint8_t* data)
{
  const USBReport01_t* r = (const USBReport01_t*)data;

  state->timestamp = r->Counter2;
  state->gyroX = r->gyroX;
  state->gyroY = r->gyroY;
  state->gyroZ = r->gyroZ;

  // Accelerator data is unused for now
}

void DS5USB::IMUStateToBuffer(const imu_state_t* state, uint8_t* data)
{
  USBReport01_t* r = (USBReport01_t*)data;

  r->gyroX = state->gyroX;
  r->gyroY = state->gyroY;
  r->gyroZ = state->gyroZ;
}
