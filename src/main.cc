#include "header.h"
#include "keymap.h"
#include "server.h"
#include "subscribe.h"
#include "util.h"
#include <thread>

#define BACKLOG 3

using namespace std;

int main(int argc, char *argv[]) {
  KeyMap keymap;
  sv_main(keymap, PORT_NUM_GET, PORT_NUM_UPDATE);

  return 0;
}