#include "server.h"
#include "header.h"
#include "server_impl.h"

using namespace std;

#define BACKLOG 4

int sv_main(KeyMap &data, int port_control, int port_update) {
#if defined(MODE_ASYNC)
#warning In ASYNC mode
  return sv_main_impl_async(data, port_control, port_update);
#elif defined(MODE_SYNC)
#warning In SYNC mode
  return sv_main_impl_sync(data, port_control, port_update);
#else
#error Please define one of MODE_ASYNC and MODE_SYNC
#endif
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