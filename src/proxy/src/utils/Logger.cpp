/**
 * @file Logger.cpp
 */

/**********************************************************************

  Created: 19 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <sstream>
#include <iostream>
#include <vector>
#include <cstring>

#ifdef USE_SPDLOG
#include <spdlog/sinks/rotating_file_sink.h>
#endif

#include "format.h"
#include "Logger.h"

namespace dsremap
{
#ifdef USE_SPDLOG
  std::shared_ptr<spdlog::sinks::sink> Logger::_sink(nullptr);
#endif

  Logger::Logger(const std::string& name)
#ifdef USE_SPDLOG
    : _logger(spdlog::get(name))
#else
    : _name(name)
#endif
  {
#ifdef USE_SPDLOG
    if (!_sink.get())
      _sink.reset(new spdlog::sinks::rotating_file_sink_st("/var/log/dsremap-proxy.log", 65536, 10));

    if (!_logger.get())
      _logger = std::make_shared<spdlog::logger>(name, _sink);
#endif
  }

  Logger::~Logger()
  {
  }

  void Logger::debug_hex(const uint8_t* ptr, size_t len, bool compact)
  {
    if (compact) {
      std::vector<char> bf(len * 2 + 1);
      for (size_t i = 0; i < len; ++i)
        sprintf(bf.data() + 2 * i, "%02X", (int)ptr[i]);
      debug("{}", bf.data());
    } else {
      for (size_t i = 0; i < (len + 31) / 32; ++i) {
        std::ostringstream oss;

        for (size_t j = 0; j < std::min((size_t)32, len - i * 32); ++j) {
          if (j != 0)
            oss << " ";
          oss << format("{:02x}", ptr[i*32+j]);
        }

        debug("{}", oss.str());
      }
    }
  }

#ifndef USE_SPDLOG
  void Logger::_message(unsigned int level, const std::string& message)
  {
    if (level >= _level) {
      char when[4096];
      time_t now = time(NULL);
      ::strftime(when, 4096, "%Y-%m-%d %H:%M:%S", ::localtime(&now));
      std::cerr << format("[{}] [{}] [{}] {}", when, _name, _level_name(level), message) << std::endl;
    }
  }

  std::string Logger::_level_name(unsigned int level)
  {
    switch (level) {
      case Level::debug:
        return "\033[33mdebug\033[0m";
      case Level::info:
        return "\033[37minfo\033[0m";
      case Level::warn:
        return "\033[33mwarn\033[0m";
      case Level::error:
        return "\033[31merror\033[0m";
    }

    return "??";
  }
#endif
}
