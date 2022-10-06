#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include "channel.h"
#include "keymap.h"
#include "parse.h"

int chnl_create(int sfd, ChannelId &chid, Line &line);
int chnl_set_control(int sfd, ChannelId &chid, Line &line);
int chnl_set_update(int sfd, ChannelId &chid, Line &line);
int chnl_del(int sfd, ChannelId &chid, Line &line);

int hget(int sfd, ChannelId &chid, KeyMap &data, Line &line);
int hset(int sfd, ChannelId &chid, KeyMap &data, Line &line);

int quit(int sfd);

#endif