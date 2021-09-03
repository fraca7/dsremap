/**
 * @file File.cpp
 */

/**********************************************************************

  Created: 15 Jul 2021

    Copyright (C) 2021 jerome@jeromelaheurte.net

**********************************************************************/

#include <stdexcept>
#include <system_error>
#include <fstream>
#include <cstring>

#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

#include "format.h"
#include "File.h"

namespace dsremap
{
  File::File(const std::string& path)
    : _path(path)
  {
  }

  File::File(const File& base, const std::string& path)
    : _path(format("{}/{}", base._path, path))
  {
  }

  bool File::exists() const
  {
    struct stat statbuf;
    if (::stat(_path.c_str(), &statbuf) == 0)
      return true;

    if (errno == ENOENT)
      return false;

    throw std::system_error(errno, std::generic_category(), format("Cannot stat() \"{}\"", _path));
  }

  void File::mkdir(int mode) const
  {
    if (::mkdir(_path.c_str(), mode) < 0)
      throw std::system_error(errno, std::generic_category(), format("Cannot mkdir \"{}\"", _path));
  }

  void File::rmdir() const
  {
    if (::rmdir(_path.c_str()) < 0)
      throw std::system_error(errno, std::generic_category(), format("Cannot rmdir \"{}\"", _path));
  }

  void File::symlink(const File& target) const
  {
    if (::symlink(_path.c_str(), target._path.c_str()) < 0)
      throw std::system_error(errno, std::generic_category(), format("Cannot link \"{}\" to \"{}\"", _path, target._path));
  }

  void File::unlink() const
  {
    if (::unlink(_path.c_str()) < 0)
      throw std::system_error(errno, std::generic_category(), format("Cannot unlink \"{}\"", _path));
  }

  std::vector<File> File::ls() const
  {
    DIR* dir = ::opendir(_path.c_str());
    if (!dir)
      throw std::system_error(errno, std::generic_category(), format("Cannot opendir \"{}\"", _path));

    std::vector<File> results;

    struct dirent* rd;
    while ((rd = ::readdir(dir))) {
      if (!::strcmp(rd->d_name, ".") || !::strcmp(rd->d_name, ".."))
        continue;
      results.push_back(File(rd->d_name));
    }
    ::closedir(dir);

    return results;
  }

  void File::echo(const std::string& value) const
  {
    std::ofstream ofs(_path, std::ios::trunc);
    ofs << value;
  }

  void File::mount(const std::string& type, const std::string& dev) const
  {
    std::string cmdline = format("/bin/mount -t {} {} {}", type, dev, _path);

    int err;
    switch ((err = ::system(cmdline.c_str()))) {
      case 0:
        break;
      case -1:
        throw std::system_error(errno, std::generic_category(), "Cannot create mount process");
      default:
        throw std::runtime_error(format("Cannot mount \"{}\": {}", _path, err));
    }
  }

  void File::unmount() const
  {
    std::string cmdline = format("/bin/umount {}", _path);
    int err;
    switch ((err = ::system(cmdline.c_str()))) {
      case 0:
        break;
      case -1:
        throw std::system_error(errno, std::generic_category(), "Cannot create umount process");
      default:
        throw std::runtime_error(format("Cannot unmount \"{}\": {}", _path, err));
    }
  }
}
