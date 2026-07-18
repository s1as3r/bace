#ifndef _H_WIN32_BASE
#define _H_WIN32_BASE

#include "bace/base.h"
#include "bace/base_os.h"
#include "bace/arena.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h> // IWYU pragma: export

typedef struct Win32State {
  Arena *arena;
  SystemInfo sys_info;
} Win32State;

void init_win32_state(void);

#endif // !_H_WIN32_BASE
