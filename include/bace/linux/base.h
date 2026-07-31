#ifndef _H_LINUX_BASE
#define _H_LINUX_BASE

#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <dirent.h>

#include "bace/base.h" // IWYU pragma: export
#include "bace/base_os.h"
#include "bace/arena.h"

typedef struct LinuxState {
  Arena *arena;
  SystemInfo sys_info;
} LinuxState;

void init_linux_state(void);

typedef struct Linux_FileIter {

  DIR *dir;
  struct dirent *dp;
  Str8 path;
} Linux_FileIter;

#endif // !_H_LINUX_BASE
