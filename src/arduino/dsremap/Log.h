
#ifndef _LOG_H
#define _LOG_H

#include "LUFAConfig.h"
#include <LUFA.h>
#include "Config.h"
#include "Utils.h"

class LogChecksum {
public:
  LogChecksum();

  void update(uint8_t);
  uint32_t finalize() const;

private:
  uint32_t m_value;
};

inline void initLog() {
#if LOG_LEVEL > LOG_LEVEL_NONE
  LOG_SERIAL .begin(LOG_SERIAL_BAUDRATE);
  while (!LOG_SERIAL);
#endif
}

#define LOG_TYPE_INT8 1
#define LOG_TYPE_UINT8 2
#define LOG_TYPE_INT16 3
#define LOG_TYPE_UINT16 4
#define LOG_TYPE_INT32 5
#define LOG_TYPE_UINT32 6
#define LOG_TYPE_FLOAT 7

class LogSender {
public:
  LogSender(uint8_t level, uint16_t msgid)
    : m_ck(), m_types(0), m_types_count(0)
  {
    m_header.magic = 0xCAFE;
    m_header.level = level;
    m_header.msgid = msgid;
    m_header.params = 0;
  }

private:
  template <typename... Args> void Prepare(Args...);
  template <typename T, typename... Args> void Prepare(const T&, Args...);

  template <typename... Args> void SendTypes(Args...);
  template <typename... Args> void SendTypes(int8_t, Args...);
  template <typename... Args> void SendTypes(uint8_t, Args...);
  template <typename... Args> void SendTypes(int16_t, Args...);
  template <typename... Args> void SendTypes(uint16_t, Args...);
  template <typename... Args> void SendTypes(int32_t, Args...);
  template <typename... Args> void SendTypes(uint32_t, Args...);
  template <typename... Args> void SendTypes(float, Args...);

  template <typename... Args> void DoSend(Args...);
  template <typename T, typename... Args> void DoSend(const T&, Args...);

#ifdef WIN32
#pragma pack(push, 1)
#endif

  struct PACKED {
    uint16_t magic;
    uint8_t level;
    uint8_t params;
    uint16_t msgid;
  } m_header;

#ifdef WIN32
#pragma pack(pop)
#endif

  LogChecksum m_ck;
  uint8_t m_types;
  uint8_t m_types_count;

  void AddType(uint8_t type) {
    switch (m_types_count) {
      case 0:
        m_types |= type;
        ++m_types_count;
        break;
      case 1:
        m_types |= type << 4;
        m_ck.update(m_types);
        LOG_SERIAL.write(&m_types, 1);
        m_types = m_types_count = 0;
        break;
    }
  }

public:
  template <typename... Args> void Send(Args... args) {
    Prepare(args...);

    for (uint8_t i = 2; i < sizeof(m_header); ++i)
      m_ck.update(((uint8_t*)&m_header)[i]);

    LOG_SERIAL.write((uint8_t*)&m_header, sizeof(m_header));

    SendTypes(args...);
    if (m_types_count) {
      m_ck.update(m_types);
      LOG_SERIAL.write(&m_types, 1);
    }

    DoSend(args...);

    auto crc = m_ck.finalize();
    LOG_SERIAL.write((uint8_t*)&crc, sizeof(crc));
  }
};

template <> inline void LogSender::Prepare() {
}

template <typename T, typename... Args> void LogSender::Prepare(const T& arg, Args... args) {
  m_header.params += 1;
  Prepare(args...);
}

template <> inline void LogSender::SendTypes() {
}

template <typename... Args> void LogSender::SendTypes(int8_t arg, Args... args) {
  AddType(LOG_TYPE_INT8);
  SendTypes(args...);
}

template <typename... Args> void LogSender::SendTypes(uint8_t arg, Args... args) {
  AddType(LOG_TYPE_UINT8);
  SendTypes(args...);
}

template <typename... Args> void LogSender::SendTypes(int16_t arg, Args... args) {
  AddType(LOG_TYPE_INT16);
  SendTypes(args...);
}

template <typename... Args> void LogSender::SendTypes(uint16_t arg, Args... args) {
  AddType(LOG_TYPE_UINT16);
  SendTypes(args...);
}

template <typename... Args> void LogSender::SendTypes(int32_t arg, Args... args) {
  AddType(LOG_TYPE_INT32);
  SendTypes(args...);
}

template <typename... Args> void LogSender::SendTypes(uint32_t arg, Args... args) {
  AddType(LOG_TYPE_UINT32);
  SendTypes(args...);
}

template <typename... Args> void LogSender::SendTypes(float arg, Args... args) {
  AddType(LOG_TYPE_FLOAT);
  SendTypes(args...);
}

template <> inline void LogSender::DoSend() {
}

template <typename T, typename... Args> void LogSender::DoSend(const T& arg, Args... args) {
  uint8_t* ptr = (uint8_t*)&arg;
  LOG_SERIAL.write(ptr, sizeof(arg));
  for (uint8_t i = 0; i < sizeof(arg); ++i)
    m_ck.update(ptr[i]);
  DoSend(args...);
}

template <typename... Args> void LogMessage(uint8_t level, uint16_t msgid, Args... args)
{
  LogSender sender(level, msgid);
  sender.Send(args...);
}

#if LOG_LEVEL >= LOG_LEVEL_TRACE

template <typename... Args> void LogTrace(uint16_t msgid, Args... args)
{
  LogMessage(LOG_LEVEL_TRACE, msgid, args...);
}

#else

template <typename... Args> void LogTrace(uint16_t msgid, Args... args)
{
}

#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG

template <typename... Args> void LogDebug(uint16_t msgid, Args... args)
{
  LogMessage(LOG_LEVEL_DEBUG, msgid, args...);
}

#else

template <typename... Args> void LogDebug(uint16_t msgid, Args... args)
{
}

#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO

template <typename... Args> void LogInfo(uint16_t msgid, Args... args)
{
  LogMessage(LOG_LEVEL_INFO, msgid, args...);
}

#else

template <typename... Args> void LogInfo(uint16_t msgid, Args... args)
{
}

#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN

template <typename... Args> void LogWarning(uint16_t msgid, Args... args)
{
  LogMessage(LOG_LEVEL_WARN, msgid, args...);
}

#else

template <typename... Args> void LogWarning(uint16_t msgid, Args... args)
{
}

#endif

#if LOG_LEVEL >= LOG_LEVEL_ERR

template <typename... Args> void LogError(uint16_t msgid, Args... args)
{
  LogMessage(LOG_LEVEL_ERR, msgid, args...);
}

#else

template <typename... Args> void LogError(uint16_t msgid, Args... args)
{
}

#endif

void LogDumpHex(int level, uint8_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace=MEMSPACE_RAM, uint16_t msgid=0xFFFF);

#if LOG_LEVEL >= LOG_LEVEL_INFO
inline void LogDumpInfo(uint8_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace=MEMSPACE_RAM, uint16_t msgid=0xFFFF) {
  LogDumpHex(LOG_LEVEL_INFO, len, buf, memspace, msgid);
}
#else
inline void LogDumpInfo(uint8_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace=MEMSPACE_RAM, uint16_t msgid=0xFFFF) {
}
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
inline void LogDump(uint8_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace=MEMSPACE_RAM, uint16_t msgid=0xFFFF) {
  LogDumpHex(LOG_LEVEL_DEBUG, len, buf, memspace, msgid);
}
#else
inline void LogDump(uint8_t len, const uint8_t* buf, USB_DescriptorMemorySpaces_t memspace=MEMSPACE_RAM, uint16_t msgid=0xFFFF) {
}
#endif

#endif /* _LOG_H */
