/**
 * @file BytecodeFile.cpp
 */

/**********************************************************************

  Created: 04 sept. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <stdexcept>
#include <fstream>

#ifdef USE_SPDLOG
#include <spdlog/spdlog.h>
#include <spdlog/fmt/bin_to_hex.h>
#endif

#include "File.h"
#include "BytecodeFile.h"

namespace dsremap
{
  BytecodeFile::BytecodeFile()
    : Logger("Bytecode"),
      _bytecode(),
      _stacksize(0)
  {
    if (exists()) {
      File config(CONFIG_PATH);
      info("Loading bytecode from {}", CONFIG_PATH);

      std::ifstream ifs(CONFIG_PATH);
      struct {
        uint16_t magic;
        uint16_t conflen;
        uint16_t actionlen;
        uint16_t stacksize;
      } header;
      ifs.read((char*)&header, sizeof(header));

      if (ifs.gcount() != sizeof(header)) {
#ifdef USE_SPDLOG
        error("Incomplete header: {}", spdlog::to_hex((uint8_t*)&header, (uint8_t*)&header + ifs.gcount()));
#endif
        throw std::runtime_error("Bytecode file too short");
      }

      if (header.magic != 0xCAFE) {
#ifdef USE_SPDLOG
        error("Invalid magic: {}", spdlog::to_hex((uint8_t*)&header, (uint8_t*)&header + ifs.gcount()));
#endif
        throw std::runtime_error("Invalid magic in bytecode file");
      }

      _stacksize = header.stacksize;
      _bytecode.resize(header.actionlen - 2);

      ifs.read((char*)_bytecode.data(), header.actionlen - 2);
      if (ifs.gcount() != header.actionlen - 2) {
#ifdef USE_SPDLOG
        error("Bytecode to short. Header: {}", spdlog::to_hex((uint8_t*)&header, (uint8_t*)&header + sizeof(header)));
        error("Bytecode: {}", spdlog::to_hex(_bytecode.data(), _bytecode.data() + ifs.gcount()));
#endif
        throw std::runtime_error("Bytecode file too short");
      }
    }
  }

  bool BytecodeFile::exists() const
  {
    File config(CONFIG_PATH);
    return config.exists();
  }

  uint8_t* BytecodeFile::bytecode() const
  {
    uint8_t* ptr = (uint8_t*)malloc(_bytecode.size());
    memcpy(ptr, _bytecode.data(), _bytecode.size());
    return ptr;
  }

  uint16_t BytecodeFile::stacksize() const
  {
    return _stacksize;
  }
}
