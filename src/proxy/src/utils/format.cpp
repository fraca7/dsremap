/**
 * @file format.cpp
 */

/**********************************************************************

  Created: 03 août 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include "format.h"

namespace dsremap
{
  namespace _minifmt_private
  {
    fmt_spec::fmt_spec()
      : base(BUILTIN),
        flags(std::ios::showpoint),
        fill(' '),
        width(0)
    {
    }

    fmt_spec::fmt_spec(const std::ostream& os)
      : base(BUILTIN),
        flags(os.flags()),
        fill(os.fill()),
        width(os.width())
    {
    }

    void fmt_spec::apply(std::ostream& os) const
    {
      os.flags(flags);
      os.fill(fill);
      os.width(width);
    }

    bool parse_next_spec(fmt_spec& spec, const char* fmt, std::streamoff offset)
    {
      unsigned int state = 0;
      while (fmt[offset]) {
        switch (state) {
          case 0: // Waiting for '{'
            switch (fmt[offset]) {
              case '{':
                spec.start = offset;
                state = 1;
                break;
              default:
                break;
            }
            break;
          case 1: // After '{'
            switch (fmt[offset]) {
              case '}':
                spec.end = offset + 1;
                return true;
              case ':':
                state = 2;
              break;
            }
            break;
          case 2: // Fill, alignment, or width
            switch (fmt[offset]) {
              case '}':
                throw std::runtime_error("Invalid format specification");
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                spec.width = fmt[offset] - '0';
                state = 4;
                break;
              case '<':
                spec.flags |= std::ios::left;
                state = 4;
                break;
              case '>':
                spec.flags |= std::ios::right;
                state = 4;
                break;
              default:
                spec.fill = fmt[offset];
                state = 3;
                break;
            }
            break;
          case 3: // Alignment or width
            switch (fmt[offset]) {
              case '<':
                spec.flags |= std::ios::left;
                state = 4;
                break;
              case '>':
                spec.flags |= std::ios::right;
                state = 4;
                break;
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                spec.width = fmt[offset] - '0';
                state = 4;
                break;
              default:
                throw std::runtime_error("Invalid format specification");
            }
            break;
          case 4: // Width
            switch (fmt[offset]) {
              case '0':
              case '1':
              case '2':
              case '3':
              case '4':
              case '5':
              case '6':
              case '7':
              case '8':
              case '9':
                spec.width *= 10;
                spec.width += fmt[offset] - '0';
                break;
              case 'x':
                spec.base = fmt_spec::BUILTIN;
                spec.flags |= std::ios::hex;
                state = 5;
                break;
              case 'd':
                spec.base = fmt_spec::BUILTIN;
                spec.flags |= std::ios::dec;
                state = 5;
                break;
              case 'b':
                spec.base = fmt_spec::BINARY;
                state = 5;
                break;
              case '}':
                spec.end = offset + 1;
                return true;
              default:
                throw std::runtime_error("Invalid format specification");
            }
            break;
          case 5:
            switch (fmt[offset]) {
              case '}':
                spec.end = offset + 1;
                return true;
              default:
                throw std::runtime_error("Invalid format specification");
            }
            break;
        }

        ++offset;
      }

      if (state != 0)
        throw std::runtime_error("Unterminated format specification");

      return false;
    }
  }
}
