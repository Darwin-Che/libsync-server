#include "server.h"
#include "channel.h"
#include "parse.h"
#include "server_helper.h"
#include "util.h"
#include <cstdio>
#include <memory>
#include <thread>

using namespace std;

#define BACKLOG 4

int sv_serve_control_exec_line(int sfd, ChannelId &chid, KeyMap &data,
                               Line &line) {
  int ret = 0;
  cout << endl;
  cout << "CONTROL INPUT : " << line.get_line()
       << " (cnt=" << line.get_words_num() << ')' << endl;
  try {
    Op op = OpParse::parse(line.get_word());
    if (op == Op::CHNL_SET) {
      ret = chnl_set_control(sfd, chid, line);
    } else if (op == Op::CHNL_CREATE) {
      ret = chnl_create(sfd, chid, line);
    } else if (op == Op::CHNL_DEL) {
      ret = chnl_del(sfd, chid, line);
    } else if (op == Op::HGET) {
      ret = hget(sfd, chid, data, line);
    } else if (op == Op::HSET) {
      ret = hset(sfd, chid, data, line);
    } else if (op == Op::QUIT) {
      ret = quit(sfd);
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

int sv_serve_update_exec_line(int sfd, ChannelId &chid, KeyMap &data,
                              Line &line) {
  int ret = 0;
  cout << endl;
  cout << "UPDATE INPUT : " << line.get_line()
       << " (cnt=" << line.get_words_num() << ')' << endl;
  try {
    Op op = OpParse::parse(line.get_word());
    if (op == Op::CHNL_SET) {
      ret = chnl_set_update(sfd, chid, line);
    } else if (op == Op::QUIT) {
      ret = quit(sfd);
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

unique_ptr<SockRepn> init_accept_sock(int port) {
  unique_ptr<SockRepn> sock = make_unique<SockRepn>();
  int optval;
  sock->fd = socket(AF_INET, SOCK_STREAM, 0);
  if (sock->fd == -1)
    Util::errExit("socket");

  memset(&sock->addr, 0, sizeof(struct sockaddr_in));
  sock->addr.sin_family = AF_INET;
  sock->addr.sin_addr.s_addr = htonl(INADDR_ANY);
  sock->addr.sin_port = htons(port);

  if (setsockopt(sock->fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
      -1)
    Util::errExit("setsockopt");

  if (::bind(sock->fd, (struct sockaddr *)&sock->addr,
             sizeof(struct sockaddr_in)) == -1) {
    Util::errExit("bind port", port);
  }

  if (listen(sock->fd, BACKLOG) == -1)
    Util::errExit("listen");

  return sock;
}

int sv_serve_control(unique_ptr<SockRepn> sv_sock, KeyMap &data) {
  char claddr_str[INET_ADDRSTRLEN];

  if (inet_ntop(AF_INET, &sv_sock->addr.sin_addr, claddr_str,
                INET_ADDRSTRLEN) == NULL) {
    cout << "cannot convert client addr" << endl;
  } else {
    cout << "CONTROL CONN FROM (" << claddr_str << ", "
         << ntohs(sv_sock->addr.sin_port) << ")" << endl;
  }

  // Prepare Channel
  ChannelId chid = 0;
  chnl_mgr.set_control_fd(sv_sock->fd, chid);

  Line line(RECV_BUF_SZ);

  while (1) {
    ssize_t len =
        Util::read_line(sv_sock->fd, line.get_line(), line.get_sz() - 1, true);
    if (len < 0)
      break;

    line.parse_words();
    int res = sv_serve_control_exec_line(sv_sock->fd, chid, data, line);

    if (res < 0)
      break;
  }

  // Cleanup Channel
  chnl_mgr.del_control_fd(sv_sock->fd, chid);

  if (close(sv_sock->fd) == -1)
    Util::errExit("close");

  cout << "closing socket" << endl << endl;
  return 0;
}
int sv_serve_update(unique_ptr<struct SockRepn> sv_sock, KeyMap &data) {
  char claddr_str[INET_ADDRSTRLEN];

  if (inet_ntop(AF_INET, &sv_sock->addr.sin_addr, claddr_str,
                INET_ADDRSTRLEN) == NULL)
    cout << "cannot convert client addr" << endl;
  else
    cout << "UPDATE CONN FROM (" << claddr_str << ", "
         << ntohs(sv_sock->addr.sin_port) << ")" << endl;

  // Prepare Channel
  ChannelId chid = 0;
  chnl_mgr.set_update_fd(sv_sock->fd, chid);

  Line line(RECV_BUF_SZ);

  while (1) {
    ssize_t len =
        Util::read_line(sv_sock->fd, line.get_line(), line.get_sz() - 1, true);
    if (len < 0)
      break;

    line.parse_words();
    int res = sv_serve_update_exec_line(sv_sock->fd, chid, data, line);

    if (res < 0)
      break;
  }

  // Cleanup Channel
  chnl_mgr.del_update_fd(sv_sock->fd, chid);

  cout << "closing socket" << endl << endl;

  if (close(sv_sock->fd) == -1)
    Util::errExit("close");

  return 0;
}

int sv_run_control(int port, KeyMap &data) {
  auto accept_sock = init_accept_sock(port);
  unique_ptr<SockRepn> sv_sock;
  for (;;) {
    sv_sock = make_unique<SockRepn>();
    sv_sock->fd = accept(accept_sock->fd, (struct sockaddr *)&sv_sock->addr,
                         &sv_sock->len);

    if (sv_sock->fd == -1)
      Util::errExit("accept");

    thread entry_thread(sv_serve_control, move(sv_sock), ref(data));
    assert(sv_sock == nullptr);
    entry_thread.detach();
  }
  return -1;
}
int sv_run_update(int port, KeyMap &data) {
  auto accept_sock = init_accept_sock(port);
  unique_ptr<SockRepn> sv_sock;
  for (;;) {
    sv_sock = make_unique<SockRepn>();
    sv_sock->fd = accept(accept_sock->fd, (struct sockaddr *)&sv_sock->addr,
                         &sv_sock->len);

    if (sv_sock->fd == -1)
      Util::errExit("accept");

    thread entry_thread(sv_serve_update, move(sv_sock), ref(data));
    assert(sv_sock == nullptr);
    entry_thread.detach();
  }
  return -1;
}
