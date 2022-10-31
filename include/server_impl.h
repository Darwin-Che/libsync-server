#ifndef SERVER_IMPL_H
#define SERVER_IMPL_H

#include "keymap.h"
#include "server.h"

int sv_main_impl_sync(KeyMap &data, int port_control, int port_update);
int sv_main_impl_async(KeyMap &data, int port_control, int port_update);

#endif