#ifndef _H_BASE_OS
#define _H_BASE_OS

#include "base.h"
#include "strings.h"

// system information stuff
typedef struct SystemInfo {
  u32 logical_processor_count;
  u64 page_size;
  u64 large_page_size;
  u64 allocation_granularity;
  Str8 machine_name;
} SystemInfo;

SystemInfo *get_system_info(void);

void bace_os_state_init(void);

// memory stuff
void *reserve_memory(u64 size);
bool commit_memory(void *ptr, u64 size);
void decommit_memory(void *ptr, u64 size);
void release_memory(void *ptr, u64 size);

void *reserve_memory_large(u64 size);
bool commit_memory_large(void *ptr, u64 size);

// dynamic lib stuff
typedef struct DyLib {
  u64 handle;
} DyLib;

DyLib lib_open(Str8 path);
void lib_close(DyLib lib);
VoidProc *lib_load_proc(DyLib lib, Str8 name);

#endif // !_H_BASE_OS
