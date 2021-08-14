
/**
 * @file File.h
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#ifndef _FILE_H
#define _FILE_H

#include <string>
#include <vector>

namespace dsremap
{
  class File
  {
  public:
    File(const std::string& path);
    File(const File& base, const std::string& path);

    bool exists() const;
    void mkdir(int mode=0700) const;
    void rmdir() const;
    void symlink(const File& target) const;
    void unlink() const;
    std::vector<File> ls() const;

    void echo(const std::string&) const;
    void mount(const std::string& type, const std::string& dev="none") const;
    void unmount() const;

    const std::string& as_string() const {
      return _path;
    }

  private:
    std::string _path;
  };
}

#endif /* _FILE_H */
