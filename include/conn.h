#ifndef CONN_H
#define CONN_H

#include "header.h"
#include "keymap.h"
#include "parse.h"

#define RECV_BUF_SZ 256

struct SockRepn {
  struct sockaddr_in addr;
  socklen_t len;
  int fd;
};

class Conn;

struct HandlerReturn {
  enum { NONE, CONN } type;
  union {
    Conn *conn;
  };
};

class Conn {
protected:
  KeyMap &data;
  SockRepn sockRepn;

public:
  Conn(KeyMap &data, SockRepn &sockRepn);
  virtual ~Conn();
  virtual HandlerReturn handler();
  int get_fd();
};

class ConnAcceptUpdate : public Conn {
public:
  ConnAcceptUpdate(KeyMap &data, SockRepn &sockRepn);
  HandlerReturn handler() override;
};

class ConnAcceptControl : public Conn {
public:
  ConnAcceptControl(KeyMap &data, SockRepn &sockRepn);
  HandlerReturn handler() override;
};

class ConnUpdate : public Conn {
  ChannelId chid = 0;
  void init();
  void tearDown();
  int handle_update(Line &line);

public:
  ConnUpdate(KeyMap &data, SockRepn &sockRepn);
  ~ConnUpdate() override;
  HandlerReturn handler() override;
};

class ConnControl : public Conn {
  ChannelId chid = 0;
  void init();
  void tearDown();
  int handle_control(Line &line);

public:
  ConnControl(KeyMap &data, SockRepn &sockRepn);
  ~ConnControl() override;
  HandlerReturn handler() override;
};

#endif