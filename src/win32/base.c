#include "bace/win32/base.h"
#include "bace/arena.h"
#include "bace/base_os.h"
#include "bace/thread_context.h"

global Win32State win32_state = {0};

void *reserve_memory(u64 size) {
  void *result = VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
  return result;
}

bool commit_memory(void *ptr, u64 size) {
  bool result = (VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0);
  return result;
}

void decommit_memory(void *ptr, u64 size) {
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

void release_memory(void *ptr, u64 size) {
  (void)size; // not needed
  VirtualFree(ptr, 0, MEM_RELEASE);
}

void *reserve_memory_large(u64 size) {
  // have to commit
  // https://learn.microsoft.com/en-gb/windows/win32/memory/large-page-support
  void *result =
      VirtualAlloc(0, size, MEM_RESERVE | MEM_COMMIT | MEM_LARGE_PAGES, PAGE_READWRITE);
  return result;
}

bool commit_memory_large(void *ptr, u64 size) {
  (void)ptr, (void)size;
  return true;
}

void init_win32_state(void) {
  SYSTEM_INFO sys_info = {0};
  GetSystemInfo(&sys_info);

  win32_state.sys_info = (SystemInfo){
      .logical_processor_count = (u32)sys_info.dwNumberOfProcessors,
      .page_size = sys_info.dwPageSize,
      .large_page_size = GetLargePageMinimum(),
      .allocation_granularity = sys_info.dwAllocationGranularity,
  };

  TCTX *tctx = tctx_alloc();
  tctx_select(tctx);

  win32_state.arena = arena_alloc();

  u8 buf[MAX_COMPUTERNAME_LENGTH + 1] = {0};
  DWORD size = MAX_COMPUTERNAME_LENGTH + 1;
  if (GetComputerNameA((char *)buf, &size)) {
    win32_state.sys_info.machine_name = str8_copy(win32_state.arena, str8(buf, size));
  }
}

SystemInfo *get_system_info(void) {
  return &win32_state.sys_info;
}

// dy lib stuff
DyLib lib_open(Str8 path) {
  HMODULE mod = LoadLibraryA((LPCSTR)path.str);
  DyLib result = {(u64)mod};
  return result;
}

void lib_close(DyLib lib) {
  HMODULE mod = (HMODULE)lib.handle;
  FreeLibrary(mod);
}

VoidProc *lib_load_proc(DyLib lib, Str8 name) {
  HMODULE mod = (HMODULE)lib.handle;
  VoidProc *result = (VoidProc *)GetProcAddress(mod, (LPCSTR)name.str);
  return result;
}
