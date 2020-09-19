
#include "Log.h"

const uint32_t PROGMEM crc_table[16] = {
  0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
  0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
  0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
  0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};

LogChecksum::LogChecksum()
  : m_value(0xFFFFFFFF)
{
}

void LogChecksum::update(uint8_t data)
{
  uint8_t tbl_idx;
  tbl_idx = m_value ^ (data >> (0 * 4));
  m_value = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (m_value >> 4);
  tbl_idx = m_value ^ (data >> (1 * 4));
  m_value = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (m_value >> 4);
}

uint32_t LogChecksum::finalize() const
{
  return ~m_value;
}

void LogDumpHex(int level, uint8_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace, uint16_t msgid)
{
#ifdef WIN32
#pragma pack(push, 1)
#endif
  struct PACKED {
    uint16_t magic;
    uint8_t level;
    uint8_t size;
    uint16_t msgid;
  } header;
#ifdef WIN32
#pragma pack(pop)
#endif

  header.magic = 0xCAFE;
  header.level = level;
  header.size = len;
  header.msgid = msgid;

  LOG_SERIAL.write((uint8_t*)&header, sizeof(header));

  LogChecksum ck;

  for (uint8_t i = 2; i < sizeof(header); ++i)
    ck.update(((uint8_t*)&header)[i]);

  for (uint8_t i = 0; i < len; ++i) {
    uint8_t byte;
    switch (memspace) {
      case MEMSPACE_RAM:
        byte = buf[i];
        break;
      case MEMSPACE_FLASH:
        byte = pgm_read_byte(buf + i);
        break;
      default:
        byte = 0x00;
        break;
    }

    ck.update(byte);

    LOG_SERIAL.write(&byte, 1);
  }

  auto crc = ck.finalize();
  LOG_SERIAL.write((uint8_t*)&crc, sizeof(crc));
}
