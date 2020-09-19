
#ifndef _BYTECODEWRITER_H
#define _BYTECODEWRITER_H

#include "Config.h"

class Host;

class BytecodeWriter
{
public:
  BytecodeWriter();

  void OnData(const uint8_t*, uint16_t);
  uint32_t GetChecksum() const;

private:
  LogChecksum m_CK;
};

#endif /* _BYTECODEWRITER_H */
