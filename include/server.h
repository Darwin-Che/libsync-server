#ifndef SERVER_H
#define SERVER_H

#include "conn.h"
#include "keymap.h"
#include <memory>

int sv_main(KeyMap &data, int port_control, int port_update);

std::unique_ptr<SockRepn> init_accept_sock(int port);

#endif