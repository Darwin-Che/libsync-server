#ifndef KEYMAP_H
#define KEYMAP_H

#include "channel.h"
#include "dedup.h"
#include "hmap.h"
#include "subscribe.h"
#include "util.h"
#include <map>
#include <mutex>
#include <string>

class KeyMapValue {
public:
  SubscriberSet subrset;
  HMap hmap;
  KeyMapValue();
  KeyMapValue(HMap _hmap);

  KeyMapValue(KeyMapValue &&other) = delete;
  KeyMapValue &operator=(KeyMapValue &&other) = delete;
  KeyMapValue(const KeyMapValue &other) = delete;
  KeyMapValue &operator=(const KeyMapValue &other) = delete;
};

/* Mimic the redis hashtable
 * Key(string) -> field(string) -> value(string)
 */
class KeyMap { // gathers all key related info to avoid double lookups
  mutable std::mutex lock;
  std::map<const char *, KeyMapValue, Util::StrCmp> data;

public:
  void write(ChannelId chid, const std::string &key, const std::string &field,
             const std::string &value);
  void write(ChannelId chid, const char *mutation);
  void write(ChannelId chid, const std::string &mutation);

  std::string read(ChannelId chid, const std::string &key,
                   const std::string &field);
  std::string read(ChannelId chid, const char *query);
  std::string read(ChannelId chid, const std::string &query);

private:
  std::string _read_tostr_nolock(const char *key, const char *field);
  std::string _read_tostr_nolock(ChannelId chid, const char *key,
                                 const char *field);
  KeyMapValue *_get_kmvalue_nolock(const char *key);
};

#endif