#ifndef CHANNEL_H
#define CHANNEL_H

// #include <boost/bimap.hpp>
#include <map>
#include <mutex>
#include <set>
#include <string>
#include <vector>

#define CHNL_ID_LIMIT 10
#define CHNL_ID_DEFAULT 0

#define FD_INVALID -1

// Channel 0/"" is the default channel that doesn't receive any updates
typedef unsigned ChannelId;

class Channel {
  ChannelId chid;
  std::string ch_name;
  int control_fd;
  int update_fd;
  std::vector<std::string> pending_msgs;

public:
  Channel(ChannelId _chid, const std::string &_ch_name);
  std::string get_ch_name();
  int set_control_fd(int fd);
  int del_control_fd(int fd);
  int set_update_fd(int fd);
  int del_update_fd(int fd);
  int send_update(const std::string &msg);
};

class ChannelMgr {
  mutable std::mutex lock;
  std::map<std::string, ChannelId> name_map;
  std::map<ChannelId, Channel> channel_map;
  // std::map<int, ChannelId> fd_control_map;
  // std::map<int, ChannelId> fd_update_map;
  std::pair<bool, ChannelId> find_unused_chid();

public:
  ChannelMgr();
  std::pair<bool, ChannelId> channel_create(const std::string &ch_name);
  std::pair<bool, ChannelId> channel_get(const std::string &ch_name) const;
  // ChannelId channel_create(const char * ch_name);
  int channel_delete(ChannelId chid);
  int channel_delete(const std::string &ch_name);

  int set_control_fd(int fd, ChannelId chid);
  int del_control_fd(int fd, ChannelId chid);
  int set_update_fd(int fd, ChannelId chid);
  int del_update_fd(int fd, ChannelId chid);

  int send_update(ChannelId chid, const std::string &msg);

private:
};

extern ChannelMgr chnl_mgr;

#endif