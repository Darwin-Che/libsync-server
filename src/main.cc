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

  // char c;
  // std::set<ChannelId> s;
  // DedupSetMgr<ChannelId> mgr;
  // DedupSet<ChannelId> dds1 = mgr.get(s);
  // DedupSet<ChannelId> dds2 = mgr.get(s);
  // std::cin >> c;
  // s.insert(1);
  // dds1 = mgr.get(s);
  // std::cin >> c;
  // s.insert(2);
  // dds2 = mgr.get(s);
  // std::cin >> c;
  // s.erase(2);
  // dds1 = mgr.get(s);
  // std::cin >> c;
  // dds2 = mgr.get(s);
  // std::cin >> c;
  // return 0;

  thread sv_control_thread(sv_run_control, PORT_NUM_GET, ref(keymap));
  thread sv_update_thread(sv_run_update, PORT_NUM_UPDATE, ref(keymap));
  sv_control_thread.join();
  sv_update_thread.join();

  return 0;
}