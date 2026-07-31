#ifndef _H_WIN32_BASE
#define _H_WIN32_BASE

#define WIN32_LEAN_AND_MEAN
#include <windows.h> // IWYU pragma: export

#include "bace/base.h"
#include "bace/base_os.h"
#include "bace/arena.h"

typedef struct Win32State {
  Arena *arena;
  SystemInfo sys_info;
} Win32State;

void init_win32_state(void);

typedef struct W32_FileIter {
  HANDLE handle;
  WIN32_FIND_DATAW find_data;
  bool is_volume_iter;
  Str8Array drive_strings;
  u64 drive_strings_iter_idx;
} W32_FileIter;

#endif // !_H_WIN32_BASE
