
/**
 * @file Logger.h
 */

/**********************************************************************

  Created: 19 juil. 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _LOGGER_H
#define _LOGGER_H

#ifdef USE_SPDLOG
#include <spdlog/spdlog.h>
#else
#include <src/utils/format.h>
#endif

namespace dsremap
{
  class Logger
  {
  public:
#ifndef USE_SPDLOG
    struct Level {
      static const unsigned int debug = 0;
      static const unsigned int info = 1;
      static const unsigned int warn = 2;
      static const unsigned int error = 3;
    };
#endif

    Logger(const std::string& name);
    virtual ~Logger();

  protected:
    template <typename... Args> void error(const char* fmts, Args... args) {
#ifdef USE_SPDLOG
      _logger->error(fmts, args...);
#else
      _message(Level::error, format(fmts, args...));
#endif
    }

    template <typename... Args> void warn(const char* fmts, Args... args) {
#ifdef USE_SPDLOG
      _logger->warn(fmts, args...);
#else
      _message(Level::warn, format(fmts, args...));
#endif
    }

    template <typename... Args> void info(const char* fmts, Args... args) {
#ifdef USE_SPDLOG
      _logger->info(fmts, args...);
#else
      _message(Level::info, format(fmts, args...));
#endif
    }

    template <typename... Args> void debug(const char* fmts, Args... args) {
#ifdef USE_SPDLOG
      _logger->debug(fmts, args...);
#else
      _message(Level::debug, format(fmts, args...));
#endif
    }

    void debug_hex(const uint8_t*, size_t, bool compact=false);

  private:
#ifdef USE_SPDLOG
    std::shared_ptr<spdlog::logger> _logger;

    static std::shared_ptr<spdlog::sinks::sink> _sink;
#else
    std::string _name;
    unsigned int _level{Level::debug};

    void _message(unsigned int level, const std::string& msg);

    static std::string _level_name(unsigned int);
#endif
  };
}

#endif /* _LOGGER_H */
