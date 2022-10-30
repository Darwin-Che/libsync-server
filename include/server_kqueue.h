#ifndef SERVER_KQUEUE_H
#define SERVER_KQUEUE_H

#include "keymap.h"
#include "server.h"

void add_to_kqueue(int kq, short filter, Conn *conn);
void del_from_kqueue(int kq, short filter, Conn *conn);

int sv_main_impl(KeyMap &data, int port_control, int port_update);

#endif