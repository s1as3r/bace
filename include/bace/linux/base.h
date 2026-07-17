#ifndef _H_LINUX_BASE
#define _H_LINUX_BASE

// clang-format off
#include <sys/mman.h>
#include <sys/sysinfo.h>

#include "bace/base.h"
#include "bace/base_os.h"
#include "bace/arena.h"
// clang-format on

typedef struct LinuxState LinuxState;
struct LinuxState {
  Arena *arena;
  SystemInfo sys_info;
};

global LinuxState linux_state = {0};
void init_linux_state(void);

#endif // !_H_LINUX_BASE
