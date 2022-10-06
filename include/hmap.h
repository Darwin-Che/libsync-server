#ifndef HMAP_H
#define HMAP_H

#include "util.h"
#include <cstring>
#include <map>
#include <string>

class HMap {
  std::map<const char *, std::string, Util::StrCmp> data;

public:
  // read_* returns " " for not found.
  std::string read_to_str(const char *key) const;
  void read_to_buf(const char *key, char *buf, size_t buf_sz) const;
  // std::string read_to_str(const std::string & key);
  // void read_to_buf(const std::string & key, char * buf, size_t buf_sz);

  // void write(const char *key, const std::string &value);
  // void write(const char *key, const char *value);
  void write(const std::string &key, const std::string &value);
};

#endif