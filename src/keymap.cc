#include "keymap.h"
#include "channel.h"
#include "header.h"
#include "util.h"
#include <iostream>

using namespace std;

KeyMapValue::KeyMapValue() {}
KeyMapValue::KeyMapValue(HMap _hmap) : hmap(_hmap) {}

void KeyMap::write(ChannelId chid, const std::string &key,
                   const std::string &field, const std::string &value) {
  const scoped_lock slock(this->lock);
  auto old_value = this->_read_tostr_nolock(chid, key.c_str(), field.c_str());
  if (old_value == value)
    return;
  char *key_alloc = new char[key.length() + 1];
  strcpy(key_alloc, key.c_str());
  auto &kmv = data[key_alloc];
  kmv.hmap.write(field, value);
  string msg = string("hset ") + key + ' ' + field + ' ' + value + '\n';
  kmv.subrset.send_update(msg);
}

string KeyMap::read(ChannelId chid, const string &key, const string &field) {
  const scoped_lock slock(this->lock);
  return this->_read_tostr_nolock(chid, key.c_str(), field.c_str());
}

std::string KeyMap::_read_tostr_nolock(const char *key, const char *field) {
  auto kmvalue_ptr = _get_kmvalue_nolock(key);
  if (kmvalue_ptr == nullptr) {
    return NIL_STR;
  }
  return kmvalue_ptr->hmap.read_to_str(field);
}

std::string KeyMap::_read_tostr_nolock(ChannelId chid, const char *key,
                                       const char *field) {
  auto kmvalue_ptr = _get_kmvalue_nolock(key);
  if (kmvalue_ptr == nullptr) {
    char *key_alloc = Util::copy_str(key);
    auto &kmv = data[key_alloc];
    if (chid != CHNL_ID_DEFAULT) {
      kmv.subrset.add(chid);
    }
    return NIL_STR;
  }
  kmvalue_ptr->subrset.add(chid);
  return kmvalue_ptr->hmap.read_to_str(field);
}

KeyMapValue *KeyMap::_get_kmvalue_nolock(const char *key) {
  auto it = data.find(key);
  if (it == data.end()) {
    return nullptr;
  }
  return (KeyMapValue *)&it->second; // cast away const
}