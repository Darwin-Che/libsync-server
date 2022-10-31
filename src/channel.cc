#include "channel.h"
#include "util.h"
#include <iostream>

using namespace std;

ChannelMgr chnl_mgr;

////////////////// Channel ///////////////////

Channel::Channel(ChannelId _chid, const std::string &_ch_name)
    : chid(_chid), ch_name(_ch_name), control_fd(FD_INVALID),
      update_fd(FD_INVALID) {}

string Channel::get_ch_name() { return ch_name; }

int Channel::set_control_fd(int fd) {
  cout << "set_control_fd " << fd << endl;
  if (control_fd != FD_INVALID || control_fd == fd) {
    return 0;
  }
  control_fd = fd;
  return 1;
}

int Channel::del_control_fd(int fd) {
  cout << "del_control_fd " << fd << endl;
  if (control_fd != fd)
    return 0;
  control_fd = FD_INVALID;
  return 1;
}

int Channel::set_update_fd(int fd) {
  cout << "set_update_fd " << fd << endl;
  if (update_fd != FD_INVALID || update_fd == fd) {
    return 0;
  }
  update_fd = fd;
  return 1;
}

int Channel::del_update_fd(int fd) {
  cout << "del_update_fd " << fd << endl;
  if (update_fd != fd)
    return 0;
  update_fd = FD_INVALID;
  return 1;
}

int Channel::send_update(const std::string &msg) {
  if (update_fd == FD_INVALID) {
    cout << "send_update " << msg << " pending" << endl;
    // Store the msg
    pending_msgs.emplace_back(msg);
    return 0;
  } else {
    cout << "send_update " << msg << " sent" << endl;
    int ret = pending_msgs.size() + 1;
    for (const auto &pending_msg : pending_msgs) {
      Util::write_str(update_fd, pending_msg.c_str());
    }
    pending_msgs.clear();
    Util::write_str(update_fd, msg.c_str());
    return ret + 1;
  }
}

////////////////// ChannelMgr ///////////////////

ChannelMgr::ChannelMgr() {}

pair<bool, ChannelId> ChannelMgr::find_unused_chid() {
  for (ChannelId chid = 1; chid < CHNL_ID_LIMIT; chid += 1) {
    if (channel_map.find(chid) == channel_map.end()) {
      return pair<bool, ChannelId>(true, chid);
    }
  }
  return pair<bool, ChannelId>(false, 0);
}

pair<bool, ChannelId> ChannelMgr::channel_create(const std::string &ch_name) {
  cout << "channel_create " << ch_name << endl;
  const scoped_lock slock(lock);
  // any exisiting channel for ch_name ?
  auto it = name_map.find(ch_name);
  if (it != name_map.end()) {
    return pair<bool, ChannelId>(true, it->second);
  }
  // any unused chid ?
  auto [succ, chid] = find_unused_chid();
  if (!succ) {
    return pair<bool, ChannelId>(false, 0);
  }
  // assign chid
  name_map[ch_name] = chid;
  channel_map.emplace(chid, Channel(chid, ch_name));
  return pair<bool, ChannelId>(true, chid);
}

pair<bool, ChannelId>
ChannelMgr::channel_get(const std::string &ch_name) const {
  cout << "channel_get " << ch_name << endl;
  const scoped_lock slock(lock);
  auto it = name_map.find(ch_name);
  if (it == name_map.end()) {
    return pair<bool, ChannelId>(false, 0);
  }
  return pair<bool, ChannelId>(true, it->second);
}

int ChannelMgr::channel_delete(ChannelId chid) {
  cout << "channel_delete " << chid << endl;
  if (chid == CHNL_ID_DEFAULT) {
    return 0;
  }
  const scoped_lock slock(lock);
  auto it = channel_map.find(chid);
  if (it == channel_map.end()) {
    return 0;
  }
  name_map.erase(it->second.get_ch_name());
  channel_map.erase(chid);
  return 1;
}

int ChannelMgr::channel_delete(const std::string &ch_name) {
  cout << "channel_delete " << ch_name << endl;
  const scoped_lock slock(lock);
  auto it = name_map.find(ch_name);
  if (it == name_map.end()) {
    return false;
  }
  channel_map.erase(it->second);
  name_map.erase(ch_name);
  return true;
}

int ChannelMgr::set_control_fd(int fd, ChannelId chid) {
  if (chid == CHNL_ID_DEFAULT) {
    return 0;
  }
  const scoped_lock slock(lock);
  auto it = channel_map.find(chid);
  if (it == channel_map.end()) {
    return false;
  }
  return it->second.set_control_fd(fd);
}

int ChannelMgr::del_control_fd(int fd, ChannelId chid) {
  if (chid == CHNL_ID_DEFAULT) {
    return 0;
  }
  const scoped_lock slock(lock);
  auto it = channel_map.find(chid);
  if (it == channel_map.end()) {
    return false;
  }
  return it->second.del_control_fd(fd);
}

int ChannelMgr::set_update_fd(int fd, ChannelId chid) {
  if (chid == CHNL_ID_DEFAULT) {
    return 0;
  }
  const scoped_lock slock(lock);
  auto it = channel_map.find(chid);
  if (it == channel_map.end()) {
    return -1;
  }
  int res = it->second.set_update_fd(fd);
  if (res < 0) {
    return -1;
  }
  // fd_update_map[fd] = chid;
  return 1;
}

int ChannelMgr::del_update_fd(int fd, ChannelId chid) {
  if (chid == CHNL_ID_DEFAULT) {
    return 0;
  }
  const scoped_lock slock(lock);
  auto it_ch = channel_map.find(chid);
  // auto it_fd = fd_update_map.find(fd);
  int ret = 0;
  if (it_ch != channel_map.end()) {
    ret = it_ch->second.del_update_fd(fd);
  }
  // if (it_fd != fd_update_map.end()) {
  //   ret = (int)fd_update_map.erase(fd);
  // }
  return ret;
}

int ChannelMgr::send_update(ChannelId chid, const std::string &msg) {
  if (chid == CHNL_ID_DEFAULT) {
    return 0;
  }
  const scoped_lock slock(lock);
  auto it = channel_map.find(chid);
  if (it == channel_map.end())
    return -1;
  it->second.send_update(msg);
  return 1;
}