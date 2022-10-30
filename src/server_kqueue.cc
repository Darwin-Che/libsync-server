#include "server_kqueue.h"
#include "conn.h"
#include "util.h"
#include <sys/event.h>
#include <vector>

using namespace std;

#define EVENT_LIMIT_N 10

void add_to_kqueue(int kq, short filter, Conn *conn) {
  cout << "add_to_kqueue kq=" << kq << " fd=" << conn->get_fd() << endl;
  struct kevent ev;
  EV_SET(&ev, conn->get_fd(), filter, EV_ADD, 0, 0, conn);
  kevent(kq, &ev, 1, nullptr, 0, nullptr);
}

int sv_main_impl(KeyMap &data, int port_control, int port_update) {
  int kq;
  if ((kq = kqueue()) == -1) {
    perror("kqueue() : ");
    Util::errExit("kqueue");
  }

  // Hook file descriptors for these two ports
  auto conn_serve_control =
      new ConnAcceptControl(data, *init_accept_sock(port_control));
  auto conn_serve_update =
      new ConnAcceptUpdate(data, *init_accept_sock(port_update));

  // // Add the two fd to ep_fd
  // Control socket
  add_to_kqueue(kq, EVFILT_READ, conn_serve_control);
  // Update socket
  add_to_kqueue(kq, EVFILT_READ, conn_serve_update);

  // start epoll_loop
  int nev;
  struct kevent evlist[EVENT_LIMIT_N]; /* events that were triggered */
  while (true) {
    cout << endl << " ... kevent() waiting" << flush;

    nev = kevent(kq, nullptr, 0, evlist, EVENT_LIMIT_N, nullptr);

    cout << " ends nev=" << nev << endl << endl;

    if (nev < 0) {
      perror("kevent()");
      exit(EXIT_FAILURE);
    } else if (nev > 0) {
      for (int i = 0; i < nev; i += 1) {
        struct kevent *kev = &evlist[i];
        // cout << "kev ident=" << kev->ident << " flags=" << kev->flags
        //      << " filter=" << kev->filter << endl;
        Conn *conn = (Conn *)evlist[i].udata;
        if (kev->flags & EV_EOF) {
          cout << "Closing socket " << conn->get_fd() << endl;
          close(conn->get_fd());
          delete conn;
          continue;
        }
        // Accept a connection -> Return a connection, add that connection
        // Read command from connection -> Return the nothing
        HandlerReturn ret = conn->handler();
        if (ret.type == HandlerReturn::CONN) {
          add_to_kqueue(kq, EVFILT_READ, ret.conn);
        }
      }
    } else {
      cout << "kevent timeout" << endl;
    }
  }

  return 0;
}