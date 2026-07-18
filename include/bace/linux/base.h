#ifndef _H_LINUX_BASE
#define _H_LINUX_BASE

#include <sys/mman.h>
#include <sys/sysinfo.h>

#include "bace/base.h"
#include "bace/base_os.h"
#include "bace/arena.h"

typedef struct LinuxState {
  Arena *arena;
  SystemInfo sys_info;
} LinuxState;

void init_linux_state(void);

#endif // !_H_LINUX_BASE
