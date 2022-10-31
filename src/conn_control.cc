#include "channel.h"
#include "conn.h"
#include "parse.h"
#include "server_helper.h"
#include "util.h"
#include <cstdio>

using namespace std;

ConnControl::ConnControl(KeyMap &data, SockRepn &sockRepn)
    : Conn(data, sockRepn) {
  init();
}

ConnControl::~ConnControl() { tearDown(); }

void ConnControl::init() {
  //   char claddr_str[INET_ADDRSTRLEN];
  //   if (inet_ntop(AF_INET, &sockRepn.addr.sin_addr, claddr_str,
  //                 INET_ADDRSTRLEN) == NULL) {
  //     cout << "cannot convert client addr" << endl;
  //   } else {
  //     cout << "CONTROL CONN FROM (" << claddr_str << ", "
  //          << ntohs(sockRepn.addr.sin_port) << ")" << endl;
  //   }
  cout << "Control connected at fd=" << get_fd() << endl;

  chnl_mgr.set_control_fd(get_fd(), chid);
}
void ConnControl::tearDown() {
  chnl_mgr.del_control_fd(get_fd(), chid);
  cout << "closing socket" << endl << endl;
}

HandlerReturn ConnControl::handler() {
  HandlerReturn ret;
  ret.type = HandlerReturn::NONE;

  Line line(RECV_BUF_SZ);

  ssize_t len =
      Util::read_line(get_fd(), line.get_line(), line.get_sz() - 1, true);
  if (len <= 0) {
    ret.type = HandlerReturn::CLOSED;
    return ret;
  }

  line.parse_words();

  if (handle_control(line) < 0) {
    ret.type = HandlerReturn::CLOSED;
  }

  return ret;
}

int ConnControl::handle_control(Line &line) {
  int ret = 0;
  cout << "CONTROL INPUT : " << line.get_line()
       << " (cnt=" << line.get_words_num() << ')' << endl;
  try {
    Op op = OpParse::parse(line.get_word());
    if (op == Op::CHNL_SET) {
      ret = chnl_set_control(get_fd(), chid, line);
    } else if (op == Op::CHNL_CREATE) {
      ret = chnl_create(get_fd(), chid, line);
    } else if (op == Op::CHNL_DEL) {
      ret = chnl_del(get_fd(), chid, line);
    } else if (op == Op::HGET) {
      ret = hget(get_fd(), chid, data, line);
    } else if (op == Op::HSET) {
      ret = hset(get_fd(), chid, data, line);
    } else if (op == Op::QUIT) {
      ret = quit(get_fd());
    } else {
      cout << "NOOP" << endl;
      ret = 0;
    }
  } catch (Line::OutOfRange err) {
    cout << err.what() << endl;
    ret = 0;
  }
  return ret;
}