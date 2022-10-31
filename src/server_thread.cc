#include "header.h"

#ifdef MODE_SYNC

#include "conn.h"
#include "server_impl.h"
#include "util.h"
#include <assert.h>
#include <thread>
#include <vector>

using namespace std;

void conn_wrapper(Conn *conn) {
  while (1) {
    HandlerReturn ret = conn->handler();
    if (ret.type == HandlerReturn::CLOSED) {
      break;
    }
  }
  cout << "Closing socket " << conn->get_fd() << endl;
  close(conn->get_fd());
  delete conn;
}

void accept_conn_wrapper(Conn *conn) {
  while (1) {
    HandlerReturn ret = conn->handler();
    assert(ret.type == HandlerReturn::CONN);
    // create thread for the connection accepted
    thread entry_thread(conn_wrapper, ret.conn);
    entry_thread.detach();
  }
  delete conn;
}

int sv_main_impl_sync(KeyMap &data, int port_control, int port_update) {
  // Hook file descriptors for these two ports
  auto conn_serve_control =
      new ConnAcceptControl(data, *init_accept_sock(port_control));
  auto conn_serve_update =
      new ConnAcceptUpdate(data, *init_accept_sock(port_update));

  thread serve_control_thread(accept_conn_wrapper, conn_serve_control);
  thread serve_update_thread(accept_conn_wrapper, conn_serve_update);
  serve_control_thread.join();
  serve_update_thread.join();
  return 0;
}

#endif