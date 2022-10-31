#include "channel.h"
#include "conn.h"
#include "parse.h"
#include "server_helper.h"
#include "util.h"
#include <cstdio>

using namespace std;

ConnUpdate::ConnUpdate(KeyMap &data, SockRepn &sockRepn)
    : Conn(data, sockRepn) {
  init();
}

ConnUpdate::~ConnUpdate() { tearDown(); }

void ConnUpdate::init() {
  // char claddr_str[INET_ADDRSTRLEN];
  // if (inet_ntop(AF_INET, &sockRepn.addr.sin_addr, claddr_str,
  //               INET_ADDRSTRLEN) == NULL) {
  //   cout << "cannot convert client addr" << endl;
  // } else {
  //   cout << "CONTROL CONN FROM (" << claddr_str << ", "
  //        << ntohs(sockRepn.addr.sin_port) << ")" << endl;
  // }
  cout << "Update connected at fd=" << get_fd() << endl;

  chnl_mgr.set_update_fd(get_fd(), chid);
}
void ConnUpdate::tearDown() {
  chnl_mgr.del_update_fd(get_fd(), chid);
  cout << "closing socket" << endl << endl;
}

HandlerReturn ConnUpdate::handler() {
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

  if (handle_update(line) < 0) {
    ret.type = HandlerReturn::CLOSED;
  }

  return ret;
}

int ConnUpdate::handle_update(Line &line) {
  int ret = 0;
  cout << "UPDATE INPUT : " << line.get_line()
       << " (cnt=" << line.get_words_num() << ')' << endl;
  try {
    Op op = OpParse::parse(line.get_word());
    if (op == Op::CHNL_SET) {
      ret = chnl_set_update(get_fd(), chid, line);
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