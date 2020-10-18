
#ifndef _VM_H
#define _VM_H

#include "DS4Structs.h"
#include "IMUIntegrator.h"
#include "opcodes.h"

class VM
{
public:
  VM(uint8_t* bytecode, bool owner, uint8_t stackSize);
  ~VM();

  void Run(USBReport01_t*, const IMUIntegrator*);

protected:
  bool Step(USBReport01_t*);

  uint8_t* m_Bytecode;

  int16_t m_SP;
  int16_t m_TH;
  int16_t m_DELTA;
  float m_IMUX;
  float m_IMUY;
  float m_IMUZ;

  uint16_t m_Offset;
  uint8_t* m_Stack;

  // Code loading
  uint8_t LoadU8();
  uint16_t LoadU16();
  int16_t LoadS16();
  int32_t LoadS32();
  float LoadF();

  // Stack
  void PushU16(uint16_t);
  uint16_t PopU16();
  void PushS16(int16_t);
  void PushS32(int32_t);
  int16_t PopS16();
  int32_t PopS32();
  void PushF(float);

private:
  bool m_bOwner;

  void StepBinary(USBReport01_t*, uint8_t);
  void StepUnary(USBReport01_t*, uint8_t);
  void StepStack(USBReport01_t*, uint8_t);
  bool StepFlow(USBReport01_t*, uint8_t);

  int32_t GetIntRegister(USBReport01_t*, int);
  void SetIntRegister(USBReport01_t*, int, int32_t);
  float GetFloatRegister(USBReport01_t*, int);

  int32_t BinaryOpInt(uint8_t, int32_t, int32_t);
  float BinaryOpFloat(uint8_t, float, float);

  int32_t LoadIntAddr(USBReport01_t*, uint8_t);
  float LoadFloatAddr(USBReport01_t*, uint8_t);
};

#endif /* _VM_H */
