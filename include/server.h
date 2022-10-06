#ifndef SERVER_H
#define SERVER_H

#include "header.h"
#include "keymap.h"

struct SockRepn {
  struct sockaddr_in addr;
  socklen_t len;
  int fd;
};

int sv_run_control(int port, KeyMap &data);
int sv_serve_control(std::unique_ptr<struct SockRepn> sv_sock, KeyMap &data);

int sv_run_update(int port, KeyMap &data);
int sv_serve_update(std::unique_ptr<struct SockRepn> sv_sock, KeyMap &data);

#define RECV_BUF_SZ 256

#endif