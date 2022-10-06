#include "server_helper.h"
#include "channel.h"
#include "keymap.h"
#include "parse.h"
#include "util.h"

using namespace std;

int chnl_set_control(int sfd, ChannelId &chid, Line &line) {
  auto &ch_name = line.get_word();
  auto [succ, _chid] = chnl_mgr.channel_get(ch_name.get_str());
  if (succ) {
    chnl_mgr.del_control_fd(sfd, chid);
    int ret = chnl_mgr.set_control_fd(sfd, _chid);
    if (ret) {
      chid = _chid;
      Util::write_str(sfd, "CHANNEL IS SET\n");
    } else {
      Util::write_str(sfd, "CHANNEL IS ALREADY CONTROLLED\n");
    }
  } else {
    Util::write_str(sfd, "CHANNEL DOES NOT EXIST\n");
  }
  return 0;
}

int chnl_set_update(int sfd, ChannelId &chid, Line &line) {
  auto &ch_name = line.get_word();
  auto [succ, _chid] = chnl_mgr.channel_get(ch_name.get_str());
  if (succ) {
    chnl_mgr.del_control_fd(sfd, chid);
    int ret = chnl_mgr.set_update_fd(sfd, _chid);
    if (ret) {
      chid = _chid;
      Util::write_str(sfd, "CHANNEL IS SET\n");
    } else {
      Util::write_str(sfd, "CHANNEL IS ALREADY SUBSCRIBED\n");
    }
  } else {
    Util::write_str(sfd, "CHANNEL DOES NOT EXIST\n");
  }
  return 0;
}

int chnl_create(int sfd, ChannelId &chid, Line &line) {
  auto &ch_name = line.get_word();
  auto [succ, _chid] = chnl_mgr.channel_create(ch_name.get_str());
  if (succ) {
    Util::write_str(sfd, "CHANNEL CREATED\n");
  } else {
    Util::write_str(sfd, "CHANNEL EXISTS\n");
  }
  return 0;
}

int chnl_del(int sfd, ChannelId &chid, Line &line) {
  auto &ch_name = line.get_word();
  auto succ = chnl_mgr.channel_delete(ch_name.get_str());
  if (succ) {
    Util::write_str(sfd, "CHANNEL DELETED\n");
  } else {
    Util::write_str(sfd, "CHANNEL DOES NOT EXIST\n");
  }
  return 0;
}

int hget(int sfd, ChannelId &chid, KeyMap &data, Line &line) {
  auto &key = line.get_word();
  auto key_str = key.get_str();
  auto &field = line.get_word();
  auto field_str = field.get_str();
  string value = data.read(chid, key_str, field_str);
  string msg = value + '\n';
  if (Util::bi_writen(sfd, msg.c_str(), msg.length()) < 0)
    Util::errExit("cannot write get request value to output");
  string logmsg = "hget(" + key_str + "," + field_str + ") = " + value;
  cout << "EXEC : " << logmsg << endl;
  return 0;
}

int hset(int sfd, ChannelId &chid, KeyMap &data, Line &line) {
  auto &key = line.get_word();
  auto key_str = key.get_str();
  auto &field = line.get_word();
  auto field_str = field.get_str();
  auto &value = line.get_word();
  auto value_str = value.get_str();
  data.write(chid, key_str, field_str, value_str);
  string msg = value_str + '\n';
  if (Util::bi_writen(sfd, msg.c_str(), msg.length()) < 0)
    Util::errExit("cannot write get request value to output");
  string logmsg = "hset " + key_str + " " + field_str + " " + value_str;
  cout << "EXEC : " << logmsg << endl;
  return 0;
}

int quit(int sfd) {
  cout << "QUIT" << endl;
  if (Util::write_str(sfd, "QUIT\n") < 0)
    Util::errExit("cannot write quit request value to output");
  return -1;
}