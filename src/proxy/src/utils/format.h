
/**
 * @file format.h
 */

/**********************************************************************

  Created: 03 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _DSREMAP_FORMAT_REPL_H
#define _DSREMAP_FORMAT_REPL_H

#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

// Minimal replacement for libfmt so that the build does not take 20 minutes on a RPi 0

namespace dsremap
{
  namespace _minifmt_private
  {
    struct fmt_spec {
      fmt_spec();
      fmt_spec(const std::ostream&);

      enum {
        BUILTIN,
        BINARY
      } base;

      std::ios::fmtflags flags;
      char fill;
      std::streamsize width;

      std::streamoff start{0};
      std::streamoff end{0};

      void apply(std::ostream&) const;

      template <typename T> typename std::enable_if<std::is_integral<T>::value>::type format(std::ostream& os, const T& value) const {
        fmt_spec prev(os);
        apply(os);

        switch (base) {
          case BUILTIN:
            os << value;
            break;
          case BINARY:
          {
            int state = 0;
            std::ostringstream res;
            for (int i = sizeof(T) * 8 - 1; i >= 0; --i) {
              switch (state) {
                case 0:
                  if ((value & (1 << i)) != 0) {
                    res << "1";
                    state = 1;
                  }
                  break;
                case 1:
                  if ((value & (1 << i)) != 0)
                    res << "1";
                  else
                    res << "0";
                  break;
              }
            }
            os << res.str();
            break;
          }
        }

        prev.apply(os);
      }

      template <typename T> typename std::enable_if<!std::is_integral<T>::value>::type format(std::ostream& os, const T& value) const {
        fmt_spec prev(os);
        apply(os);
        os << value;
        prev.apply(os);
      }
    };

    bool parse_next_spec(fmt_spec&, const char*, std::streamoff);

    template <typename T> void format_one(std::ostream& os, const fmt_spec& spec, const T& value) {
      spec.format(os, value);
    }

    template <> inline void format_one<char>(std::ostream& os, const fmt_spec& spec, const char& value) {
      spec.format(os, (int)value);
    }

    template <> inline void format_one<unsigned char>(std::ostream& os, const fmt_spec& spec, const unsigned char& value) {
      spec.format(os, (unsigned int)value);
    }

    template <typename... Args> void format(std::ostream& os, const char* fmt, std::streamoff offset, Args... args);

    template <> inline void format(std::ostream& os, const char* fmt, std::streamoff offset) {
      fmt_spec spec;
      if (parse_next_spec(spec, fmt, offset))
        throw std::runtime_error("Not enough arguments for format string");
      os << fmt + offset;
    }

    template <typename V, typename... Args> void format(std::ostream& os, const char* fmt, std::streamoff offset, const V& value, Args... args) {
      fmt_spec spec;
      if (!parse_next_spec(spec, fmt, offset))
        throw std::runtime_error("Too many arguments for format string");

      os << std::string(fmt + offset, fmt + spec.start);

      format_one(os, spec, value);

      format(os, fmt, spec.end, args...);
    }
  }

  template <typename... Args> std::string format(const char* fmt, Args... args) {
    std::ostringstream oss;
    _minifmt_private::format(oss, fmt, 0, args...);
    return oss.str();
  }
}

#endif /* _DSREMAP_FORMAT_REPL_H */
