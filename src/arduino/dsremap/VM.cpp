
#ifdef TARGET_PC
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#else
#include "Config.h"
#endif

#include <assert.h>

#include "VM.h"

#define OPCODE_TYPE(x) (((x) >> 6) & 0b11)
#define OPCODE_SUBTYPE(x) (((x) >> 2) & 0b1111)
#define OPCODE_VARIANT(x) ((x) & 0b11)

#define ADDR_TYPE(x) (((x) >> 14) & 0b11)

#ifdef __arm__

static void set_float(uint8_t* dst, float value)
{
  union {
    float f;
    uint8_t u[4];
  } v;

  v.f = value;
  memcpy(dst, v.u, 4);
}

static float get_float(uint8_t* src)
{
  union {
    float f;
    uint8_t u[4];
  } v;
  memcpy(v.u, src, 4);
  return v.f;
}

#else

static void set_float(uint8_t* dst, float value)
{
  *((float*)dst) = value;
}

static float get_float(uint8_t* src)
{
  return *((float*)src);
}

#endif

VM::VM(uint8_t* bytecode, bool owner, uint8_t stackSize)
  : m_Bytecode(bytecode),
    m_SP(0),
    m_TH(0),
    m_Offset(0),
    m_Stack((uint8_t*)malloc(stackSize)),
    m_bOwner(owner)
{
}

VM::~VM()
{
  free(m_Stack);
  if (m_bOwner)
    free(m_Bytecode);
}

void VM::Run(controller_state_t* report, const IMUIntegrator* pIMU)
{
  if (pIMU->Delta()) {
    m_DELTA = pIMU->Delta();

    const Vector3D& angles = pIMU->CurrentAngles();
    m_IMUX = angles.x;
    m_IMUY = angles.y;
    m_IMUZ = angles.z;

    const Vector3D& accel = pIMU->CurrentAnglesAccel();
    m_AccelX = accel.x;
    m_AccelY = accel.y;
    m_AccelZ = accel.z;

    while (!Step(report));
  }
}

bool VM::Step(controller_state_t* report)
{
  bool ret = false;

  uint8_t opcode = LoadU8();
  switch (OPCODE_TYPE(opcode)) {
    case OPCODE_TYPE_BINARY:
      StepBinary(report, opcode);
      break;
    case OPCODE_TYPE_UNARY:
      StepUnary(report, opcode);
      break;
    case OPCODE_TYPE_STACK:
      StepStack(report, opcode);
      break;
    case OPCODE_TYPE_FLOW:
      ret = StepFlow(report, opcode);
      break;
    default:
      break;
  }

  return ret;
}

void VM::StepBinary(controller_state_t* report, uint8_t opcode)
{
  uint16_t dst = (uint16_t)LoadU8() << 8;

  switch (ADDR_TYPE(dst)) {
    case ADDR_TYPE_REG:
    {
      int index = (dst >> 8) & 0b00111111;
      int32_t op1 = GetIntRegister(report, index);

      if (OPCODE_SUBTYPE(opcode) == OPCODE_SUBTYPE_BINARY_CAST) {
        float op2 = LoadFloatAddr(report, opcode);
        SetIntRegister(report, index, op2);
      } else {
        int32_t op2 = LoadIntAddr(report, opcode);
        SetIntRegister(report, index, BinaryOpInt(opcode, op1, op2));
      }

      break;
    }
    case ADDR_TYPE_REGOFF:
    {
      dst |= LoadU8();

      int index = (dst >> 12) & 0b11;
      int sign = (dst >> 10) & 0b1;
      int value = dst & 0b1111111111;
      if (sign)
        value = -value;

      int offset = GetIntRegister(report, index) + value;
      int type = (dst >> 11) & 0b1;
      switch (type) {
        case ADDR_VALTYPE_INT:
        {
          if (OPCODE_SUBTYPE(opcode) == OPCODE_SUBTYPE_BINARY_CAST) {
            float op2 = LoadFloatAddr(report, opcode);
            *((int32_t*)(m_Stack + offset)) = op2;
          } else {
            int32_t op1 = *((int32_t*)(m_Stack + offset));
            int32_t op2 = LoadIntAddr(report, opcode);
            *((int32_t*)(m_Stack + offset)) = BinaryOpInt(opcode, op1, op2);
          }
          break;
        }
        case ADDR_VALTYPE_FLOAT:
        {
          if (OPCODE_SUBTYPE(opcode) == OPCODE_SUBTYPE_BINARY_CAST) {
            set_float(m_Stack + offset, LoadIntAddr(report, opcode));
          } else {
            float op1 = get_float(m_Stack + offset);
            float op2 = LoadFloatAddr(report, opcode);
            set_float(m_Stack + offset, BinaryOpFloat(opcode, op1, op2));
          }
          break;
        }
        default:
          break;
      }

      break;
    }
    default:
      break;
  }
}

void VM::StepUnary(controller_state_t* report, uint8_t opcode)
{
  uint16_t addr = (uint16_t)LoadU8() << 8;
  int addrtype = ADDR_TYPE(addr);

  if (addrtype != ADDR_TYPE_REG)
    addr |= LoadU8();

  int type = ADDR_VALTYPE_INT;
  switch (addrtype) {
    case ADDR_TYPE_REGOFF:
      type = (addr >> 11) & 0b1;
      break;
    default:
      break;
  }

  int subtype = OPCODE_SUBTYPE(opcode);

  switch (type) {
    case ADDR_VALTYPE_INT:
    {
      switch (addrtype) {
        case ADDR_TYPE_REG:
        {
          int32_t val = GetIntRegister(report, (addr >> 8) & 0b111111);
          switch (subtype) {
            case OPCODE_SUBTYPE_UNARY_NEG:
              val = -val;
              break;
            case OPCODE_SUBTYPE_UNARY_NOT:
              val = !val;
              break;
            default:
              break;
          }
          SetIntRegister(report, (addr >> 8) & 0b111111, val);
          break;
        }
        case ADDR_TYPE_REGOFF:
        {
          int offset = GetIntRegister(report, (addr >> 12) & 0b11);
          int delta = addr & 0b1111111111;
          if ((addr >> 10) & 0b1)
            delta = -delta;
          offset += delta;
          int32_t val = *((int32_t*)(m_Stack + offset));
          switch (subtype) {
            case OPCODE_SUBTYPE_UNARY_NEG:
              val = -val;
              break;
            case OPCODE_SUBTYPE_UNARY_NOT:
              val = !val;
              break;
            default:
              break;
          }
          *((int32_t*)(m_Stack + offset)) = val;
          break;
        }
        default:
          break;
      }
      break;
    }
    case ADDR_VALTYPE_FLOAT:
    {
      switch (addrtype) {
        case ADDR_TYPE_REG:
        {
          assert(0);
          break;
        }
        case ADDR_TYPE_REGOFF:
        {
          int offset = GetIntRegister(report, (addr >> 12) & 0b11);
          int delta = addr & 0b1111111111;
          if ((addr >> 10) & 0b1)
            delta = -delta;
          offset += delta;
          float val = get_float(m_Stack + offset);
          switch (subtype) {
            case OPCODE_SUBTYPE_UNARY_NEG:
              val = -val;
              break;
            case OPCODE_SUBTYPE_UNARY_NOT:
              val = !val;
              break;
            default:
              break;
          }
          set_float(m_Stack + offset, val);
          break;
        }
        default:
          break;
      }
      break;
    }
    default:
      break;
  }
}

void VM::StepStack(controller_state_t* report, uint8_t opcode)
{
  switch (OPCODE_SUBTYPE(opcode)) {
    case OPCODE_SUBTYPE_STACK_POP:
    {
      int index = LoadU8() & 0b111111;
      int32_t value = PopS32();
      value = CLAMPS16(value);

      switch (index) {
        case REGINDEX_SP:
          m_SP = value;
          break;
        case REGINDEX_TH:
          m_TH = value;
          break;
        default:
          assert(0);
      }

      break;
    }
    case OPCODE_SUBTYPE_STACK_PUSHI:
      PushS32(LoadS32());
      break;
    case OPCODE_SUBTYPE_STACK_PUSHF:
      PushF(LoadF());
      break;
    case OPCODE_SUBTYPE_STACK_PUSH:
    {
      uint16_t addr = (uint16_t)LoadU8() << 8;
      switch (ADDR_TYPE(addr)) {
        case ADDR_TYPE_REG:
        {
          int index = (addr >> 8) & 0b00111111;
          switch (index) {
            case REGINDEX_IMUX:
            case REGINDEX_IMUY:
            case REGINDEX_IMUZ:
            case REGINDEX_ACCELX:
            case REGINDEX_ACCELY:
            case REGINDEX_ACCELZ:
              PushF(GetFloatRegister(report, index));
              break;
            default:
              PushS32(GetIntRegister(report, index));
              break;
          }
          break;
        }
        case ADDR_TYPE_REGOFF:
        {
          addr |= LoadU8();
          int offset = addr & 0b0000001111111111;
          if (((addr >> 10) & 1) != 0)
            offset = -offset;
          offset += GetIntRegister(report, (addr >> 12) & 0b11);
          switch ((addr >> 11) & 1) {
            case ADDR_VALTYPE_INT:
              PushS32(*((int32_t*)(m_Stack + offset)));
              break;
            case ADDR_VALTYPE_FLOAT:
              PushF(get_float(m_Stack + offset));
              break;
          }
          break;
        }
      }
      break;
    }
    default:
      break;
  }
}

bool VM::StepFlow(controller_state_t* report, uint8_t opcode)
{
  bool ret = false;

  switch (OPCODE_SUBTYPE(opcode)) {
    case OPCODE_SUBTYPE_FLOW_JUMP:
      m_Offset = LoadU16();
      break;
    case OPCODE_SUBTYPE_FLOW_YIELD:
      ret = true;
      break;
    case OPCODE_SUBTYPE_FLOW_CALL:
    {
      uint16_t offset = LoadU16();
      PushU16(m_Offset);
      m_Offset = offset;

      break;
    }
    case OPCODE_SUBTYPE_FLOW_RET:
      m_Offset = PopU16();
      break;
    case OPCODE_SUBTYPE_FLOW_JZ:
    {
      bool jump = false;

      switch (OPCODE_VARIANT(opcode)) {
        case OPCODE_VARIANT_A:
        {
          uint16_t addr = (uint16_t)LoadU8() << 8;

          switch (ADDR_TYPE(addr)) {
            case ADDR_TYPE_REGOFF:
            {
              addr |= LoadU8();
              int index = (addr >> 12) & 0b11;
              int offset = addr & 0b1111111111;
              if ((addr >> 10) & 1)
                offset = -offset;
              offset += GetIntRegister(report, index);

              switch ((addr >> 11) & 1) {
                case ADDR_VALTYPE_INT:
                  jump = *((int32_t*)(m_Stack + offset)) == 0;
                  break;
                case ADDR_VALTYPE_FLOAT:
                  jump = get_float(m_Stack + offset) == 0.0f;
                  break;
              }

              break;
            }
            case ADDR_TYPE_REG:
            {
              int index = (addr >> 8) & 0b111111;
              switch (index) {
                case REGINDEX_IMUX:
                case REGINDEX_IMUY:
                case REGINDEX_IMUZ:
                case REGINDEX_ACCELX:
                case REGINDEX_ACCELY:
                case REGINDEX_ACCELZ:
                  jump = GetFloatRegister(report, index) == 0.0;
                  break;
                default:
                  jump = GetIntRegister(report, index) == 0;
                  break;
              }

              break;
            }

            break;
          }

          break;
        }
        case OPCODE_VARIANT_CI:
          jump = LoadS32() == 0;
          break;
        case OPCODE_VARIANT_CF:
          jump = LoadF() == 0.0;
          break;
      }

      uint16_t offset = LoadU16();
      if (jump)
        m_Offset = offset;

      break;
    }
  }

  return ret;
}

uint8_t VM::LoadU8()
{
  return *(m_Bytecode + m_Offset++);
}

uint16_t VM::LoadU16()
{
  uint16_t v = *((uint16_t*)(m_Bytecode + m_Offset));
  m_Offset += 2;
  return v;
}

int16_t VM::LoadS16()
{
  int16_t v = *((int16_t*)(m_Bytecode + m_Offset));
  m_Offset += 2;
  return v;
}

int32_t VM::LoadS32()
{
  int32_t v = *((int32_t*)(m_Bytecode + m_Offset));
  m_Offset += 4;
  return v;
}

float VM::LoadF()
{
  float v = get_float(m_Bytecode + m_Offset);
  m_Offset += 4;
  return v;
}

void VM::PushU16(uint16_t v)
{
  *((uint16_t*)(m_Stack + m_SP)) = v;
  m_SP += 2;
}

uint16_t VM::PopU16()
{
  m_SP -= 2;
  return *((uint16_t*)(m_Stack + m_SP));
}

void VM::PushS16(int16_t v)
{
  *((int16_t*)(m_Stack + m_SP)) = v;
  m_SP += 2;
}

void VM::PushS32(int32_t v)
{
  *((int32_t*)(m_Stack + m_SP)) = v;
  m_SP += 4;
}

int16_t VM::PopS16()
{
  m_SP -= 2;
  return *((int16_t*)(m_Stack + m_SP));
}

int32_t VM::PopS32()
{
  m_SP -= 4;
  return *((int32_t*)(m_Stack + m_SP));
}

void VM::PushF(float v)
{
  set_float(m_Stack + m_SP, v);
  m_SP += sizeof(v);
}

int32_t VM::GetIntRegister(controller_state_t* report, int index)
{
  switch (index) {
    case REGINDEX_ZR:
      return 0;
    case REGINDEX_SP:
      return m_SP;
    case REGINDEX_TH:
      return m_TH;
    case REGINDEX_LPADX:
      return report->LPadX;
    case REGINDEX_LPADY:
      return report->LPadY;
    case REGINDEX_RPADX:
      return report->RPadX;
    case REGINDEX_RPADY:
      return report->RPadY;
    case REGINDEX_HAT:
      return report->Hat;
    case REGINDEX_SQUARE:
      return report->Square;
    case REGINDEX_CROSS:
      return report->Cross;
    case REGINDEX_CIRCLE:
      return report->Circle;
    case REGINDEX_TRIANGLE:
      return report->Triangle;
    case REGINDEX_L1:
      return report->L1;
    case REGINDEX_L2:
      return report->L2;
    case REGINDEX_R1:
      return report->R1;
    case REGINDEX_R2:
      return report->R2;
    case REGINDEX_SHARE:
      return report->Share;
    case REGINDEX_OPTIONS:
      return report->Options;
    case REGINDEX_L3:
      return report->L3;
    case REGINDEX_R3:
      return report->R3;
    case REGINDEX_PS:
      return report->PS;
    case REGINDEX_TPAD:
      return report->TPad;
    case REGINDEX_L2VALUE:
      return report->L2Value;
    case REGINDEX_R2VALUE:
      return report->R2Value;
    case REGINDEX_DELTA:
      return m_DELTA;
  }

  assert(0);
  return 0;
}

void VM::SetIntRegister(controller_state_t* report, int index, int32_t value)
{
  switch (index) {
    case REGINDEX_LPADX:
    case REGINDEX_LPADY:
    case REGINDEX_RPADX:
    case REGINDEX_RPADY:
    case REGINDEX_L2VALUE:
    case REGINDEX_R2VALUE:
      value = CLAMPU8(value);
      break;
    case REGINDEX_HAT:
      value = (value < 0) ? 0 : (value > 8) ? 8 : value;
      break;
    case REGINDEX_SQUARE:
    case REGINDEX_CROSS:
    case REGINDEX_CIRCLE:
    case REGINDEX_TRIANGLE:
    case REGINDEX_L1:
    case REGINDEX_L2:
    case REGINDEX_R1:
    case REGINDEX_R2:
    case REGINDEX_SHARE:
    case REGINDEX_OPTIONS:
    case REGINDEX_L3:
    case REGINDEX_R3:
    case REGINDEX_PS:
    case REGINDEX_TPAD:
      value = CLAMPU1(value);
      break;
    default:
      value = CLAMPS16(value);
      break;
  }

  switch (index) {
    case REGINDEX_SP:
      m_SP = value;
      break;
    case REGINDEX_TH:
      m_TH = value;
      break;
    case REGINDEX_LPADX:
      report->LPadX = value;
      break;
    case REGINDEX_LPADY:
      report->LPadY = value;
      break;
    case REGINDEX_RPADX:
      report->RPadX = value;
      break;
    case REGINDEX_RPADY:
      report->RPadY = value;
      break;
    case REGINDEX_HAT:
      report->Hat = value;
      break;
    case REGINDEX_SQUARE:
      report->Square = value;
      break;
    case REGINDEX_CROSS:
      report->Cross = value;
      break;
    case REGINDEX_CIRCLE:
      report->Circle = value;
      break;
    case REGINDEX_TRIANGLE:
      report->Triangle = value;
      break;
    case REGINDEX_L1:
      report->L1 = value;
      break;
    case REGINDEX_L2:
      report->L2 = value;
      break;
    case REGINDEX_R1:
      report->R1 = value;
      break;
    case REGINDEX_R2:
      report->R2 = value;
      break;
    case REGINDEX_SHARE:
      report->Share = value;
      break;
    case REGINDEX_OPTIONS:
      report->Options = value;
      break;
    case REGINDEX_L3:
      report->L3 = value;
      break;
    case REGINDEX_R3:
      report->R3 = value;
      break;
    case REGINDEX_PS:
      report->PS = value;
      break;
    case REGINDEX_TPAD:
      report->TPad = value;
      break;
    case REGINDEX_L2VALUE:
      report->L2Value = value;
      break;
    case REGINDEX_R2VALUE:
      report->R2Value = value;
      break;
    default:
      assert(0);
      break;
  }
}

float VM::GetFloatRegister(controller_state_t* report, int index)
{
  switch (index) {
    case REGINDEX_IMUX:
      return m_IMUX;
    case REGINDEX_IMUY:
      return m_IMUY;
    case REGINDEX_IMUZ:
      return m_IMUZ;
    case REGINDEX_ACCELX:
      return m_AccelX;
    case REGINDEX_ACCELY:
      return m_AccelY;
    case REGINDEX_ACCELZ:
      return m_AccelZ;
    default:
      break;
  }

  return 0.0f;
}

int32_t VM::BinaryOpInt(uint8_t opcode, int32_t op1, int32_t op2)
{
  switch (OPCODE_SUBTYPE(opcode)) {
    case OPCODE_SUBTYPE_BINARY_ADD:
      op1 += op2;
      break;
    case OPCODE_SUBTYPE_BINARY_SUB:
      op1 -= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_MUL:
      op1 *= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_DIV:
      op1 /= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_LOAD:
      op1 = op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CLT:
      op1 = op1 < op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CGT:
      op1 = op1 > op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CLTE:
      op1 = op1 <= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CGTE:
      op1 = op1 >= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CE:
      op1 = op1 == op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CN:
      op1 = op1 != op2;
      break;
    case OPCODE_SUBTYPE_BINARY_AND:
      op1 = op1 && op2;
      break;
    case OPCODE_SUBTYPE_BINARY_OR:
      op1 = op1 || op2;
      break;
  }

  return op1;
}

float VM::BinaryOpFloat(uint8_t opcode, float op1, float op2)
{
  switch (OPCODE_SUBTYPE(opcode)) {
    case OPCODE_SUBTYPE_BINARY_ADD:
      op1 += op2;
      break;
    case OPCODE_SUBTYPE_BINARY_SUB:
      op1 -= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_MUL:
      op1 *= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_DIV:
      op1 /= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_LOAD:
      op1 = op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CLT:
      op1 = op1 < op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CGT:
      op1 = op1 > op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CLTE:
      op1 = op1 <= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CGTE:
      op1 = op1 >= op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CE:
      op1 = op1 == op2;
      break;
    case OPCODE_SUBTYPE_BINARY_CN:
      op1 = op1 != op2;
      break;
    case OPCODE_SUBTYPE_BINARY_AND:
      op1 = op1 && op2;
      break;
    case OPCODE_SUBTYPE_BINARY_OR:
      op1 = op1 || op2;
      break;
  }

  return op1;
}

int32_t VM::LoadIntAddr(controller_state_t* report, uint8_t opcode)
{
  switch (OPCODE_VARIANT(opcode)) {
    case OPCODE_VARIANT_C:
      return LoadS32();
    case OPCODE_VARIANT_A:
    {
      uint16_t addr = (uint16_t)LoadU8() << 8;

      switch (ADDR_TYPE(addr)) {
        case ADDR_TYPE_REG:
          return GetIntRegister(report, (addr >> 8) & 0b111111);
        case ADDR_TYPE_REGOFF:
        {
          addr |= LoadU8();
          assert(((addr >> 11) & 1) == ADDR_VALTYPE_INT);
          int value = addr & 0b1111111111;
          if ((addr >> 10) & 1)
            value = -value;
          int offset = GetIntRegister(report, (addr >> 12) & 0b11) + value;
          return *((int32_t*)(m_Stack + offset));
        }
        default:
          break;
      }

      break;
    }
    default:
      break;
  }

  return 0;
}

float VM::LoadFloatAddr(controller_state_t* report, uint8_t opcode)
{
  switch (OPCODE_VARIANT(opcode)) {
    case OPCODE_VARIANT_C:
      return LoadF();
    case OPCODE_VARIANT_A:
    {
      uint16_t addr = (uint16_t)LoadU8() << 8;

      switch (ADDR_TYPE(addr)) {
        case ADDR_TYPE_REG:
          return GetFloatRegister(report, (addr >> 8) & 0b111111);
        case ADDR_TYPE_REGOFF:
        {
          addr |= LoadU8();
          assert(((addr >> 11) & 1) == ADDR_VALTYPE_FLOAT);
          int value = addr & 0b1111111111;
          if ((addr >> 10) & 1)
            value = -value;
          int offset = GetIntRegister(report, (addr >> 12) & 0b11) + value;
          return get_float(m_Stack + offset);
        }
        default:
          break;
      }

      break;
    }
    default:
      break;
  }

  return 0.0f;
}
