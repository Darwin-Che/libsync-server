#include "subscribe.h"
#include <iostream>

using namespace std;

SubscriberSet::SubscriberSet() : bitset(0) {}

SubscriberSet::SubscriberSet(unsigned _bitset) : bitset(_bitset) {}

bool SubscriberSet::add(ChannelId chid) {
  cout << "SubscriberSet::add " << chid << endl;
  bitset |= (1 << chid);
  return true;
}

void SubscriberSet::send_update(const std::string &msg) {
  send_update(msg.c_str());
}

void SubscriberSet::send_update(const char *msg) {
  for (ChannelId chid = 0; chid <= CHNL_ID_LIMIT; chid += 1) {
    if (bitset & (1 << chid)) {
      chnl_mgr.send_update(chid, msg);
    }
  }
}
