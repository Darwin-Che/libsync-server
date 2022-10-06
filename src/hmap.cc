#include "hmap.h"
#include "header.h"

using namespace std;

string HMap::read_to_str(const char *key) const {
  auto it = data.find(key);
  if (it == data.end())
    return NIL_STR;
  return it->second;
}
void HMap::read_to_buf(const char *key, char *buf, size_t buf_sz) const {
  auto it = data.find(key);
  if (it == data.end()) {
    assert(buf_sz > 1);
    buf[0] = NIL_CHAR;
    buf[1] = '\0';
    return;
  }
  int len = it->second.length();
  assert(buf_sz > len);
  strcpy(buf, it->second.c_str());
}

// void HMap::write(const char *key, const string &value) { data[key] = value; }
// void HMap::write(const char *key, const char *value) { data[key] = value; }

void HMap::write(const std::string &key, const std::string &value) {
  char *key_alloc = new char[key.length() + 1];
  strcpy(key_alloc, key.c_str());
  data[key_alloc] = value;
}