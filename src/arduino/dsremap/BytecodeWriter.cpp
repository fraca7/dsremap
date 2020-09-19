
#include "Host.h"
#include "BytecodeWriter.h"

#include <EEPROM.h>

BytecodeWriter::BytecodeWriter()
  : m_CK()
{
}

void BytecodeWriter::OnData(const uint8_t* data, uint16_t len)
{
  uint16_t offset = *((uint16_t*)(data + 1));
  uint16_t length = *((uint16_t*)(data + 3));

  LogInfo(BYTECODE_RECEIVED, length, offset);

  for (uint16_t i = 0; i < length; ++i) {
    m_CK.update(data[i + 5]);
    EEPROM.write(offset + i, data[i + 5]);
  }
}

uint32_t BytecodeWriter::GetChecksum() const
{
  return m_CK.finalize();
}
