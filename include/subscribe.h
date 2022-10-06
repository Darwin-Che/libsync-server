#ifndef SUBSCRIBE_H
#define SUBSCRIBE_H

#include "channel.h"
#include "dedup.h"
#include <set>

class SubscriberSet {
  unsigned bitset;

public:
  SubscriberSet();
  SubscriberSet(unsigned _bitset);
  SubscriberSet(SubscriberSet &&other) = delete;
  SubscriberSet &operator=(SubscriberSet &&other) = delete;
  SubscriberSet(const SubscriberSet &other) = delete;
  SubscriberSet &operator=(const SubscriberSet &other) = delete;

  void send_update(const std::string &msg);
  void send_update(const char *msg);

  bool add(ChannelId);
};

#endif
