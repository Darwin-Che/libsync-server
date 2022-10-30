#include "conn.h"
#include "parse.h"
#include "server_helper.h"

using namespace std;

Conn::Conn(KeyMap &_data, SockRepn &_sockRepn)
    : data(_data), sockRepn(_sockRepn) {}

Conn::~Conn() {}

ConnAcceptUpdate::ConnAcceptUpdate(KeyMap &data, SockRepn &sockRepn)
    : Conn(data, sockRepn) {}

ConnAcceptControl::ConnAcceptControl(KeyMap &data, SockRepn &sockRepn)
    : Conn(data, sockRepn) {}

int Conn::get_fd() { return sockRepn.fd; }

HandlerReturn Conn::handler() {
  Util::errExit("Conn::hanlder is not implemented");

  HandlerReturn ret;
  ret.type = HandlerReturn::NONE;
  return ret;
}

HandlerReturn ConnAcceptUpdate::handler() {
  SockRepn retSock;
  retSock.fd = accept(get_fd(), (struct sockaddr *)&retSock.addr, &retSock.len);

  if (retSock.fd == -1)
    Util::errExit("accept");

  HandlerReturn ret;
  ret.conn = new ConnUpdate(data, retSock);
  ret.type = HandlerReturn::CONN;

  return ret;
}

HandlerReturn ConnAcceptControl::handler() {
  SockRepn retSock;
  retSock.fd = accept(get_fd(), (struct sockaddr *)&retSock.addr, &retSock.len);

  if (retSock.fd == -1)
    Util::errExit("accept");

  HandlerReturn ret;
  ret.conn = new ConnControl(data, retSock);
  ret.type = HandlerReturn::CONN;

  return ret;
}