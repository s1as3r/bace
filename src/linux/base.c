#include "bace/base.h"
#include "bace/base_os.h"
#include "bace/thread_context.h"
#include "bace/linux/base.h"

#include <unistd.h>
#include <string.h>
#include <dlfcn.h>

global LinuxState linux_state = {0};

void *reserve_memory(u64 size) {
  void *res = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  if (res == MAP_FAILED) {
    res = 0;
  }
  return res;
}

bool commit_memory(void *ptr, u64 size) {
  i32 res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
  return res == 0 ? true : false;
}

void decommit_memory(void *ptr, u64 size) {
  madvise(ptr, size, MADV_DONTNEED);
  mprotect(ptr, size, PROT_NONE);
}

void release_memory(void *ptr, u64 size) {
  munmap(ptr, size);
}

void *reserve_memory_large(u64 size) {
  void *res = mmap(0, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
  if (res == MAP_FAILED) {
    res = 0;
  }
  return res;
}

bool commit_memory_large(void *ptr, u64 size) {
  i32 res = mprotect(ptr, size, PROT_READ | PROT_WRITE);
  return res == 0 ? true : false;
}

void init_linux_state(void) {
  linux_state.sys_info = (SystemInfo){
      .logical_processor_count = (u32)get_nprocs(),
      .page_size = (u64)getpagesize(),
      .large_page_size = MB(2),
      .allocation_granularity = (u64)getpagesize(),
  };
  linux_state.arena = arena_alloc();

  TCTX *tctx = tctx_alloc();
  tctx_select(tctx);

  bool got_result = 0;
  u8 *buf = 0;
  u64 size = 0;

  Temp scratch = scratch_begin(0, 0);
  for (u64 cap = 4096, r = 0; r < 4; cap *= 2, r += 1) {
    scratch_end(scratch);
    buf = push_array(scratch.arena, u8, cap);
    i32 gethostname_res = gethostname((char *)buf, cap);
    size = cstring8_length(buf);
    if (gethostname_res == 0 && size < cap) {
      got_result = 1;
      break;
    }
  }
  if (got_result && size > 0) {
    Str8 *mname = &linux_state.sys_info.machine_name;
    mname->size = size;
    mname->str = push_array_no_zero(linux_state.arena, u8, size + 1);
    memmove(mname->str, buf, size);
    mname->str[mname->size] = 0;
  }
  scratch_end(scratch);
}

SystemInfo *get_system_info(void) {
  return &linux_state.sys_info;
}

void bace_os_state_init(void) {
  init_linux_state();
}

// dy lib stuff
DyLib lib_open(Str8 path) {
  void *so = dlopen((char *)path.str, RTLD_LAZY | RTLD_LOCAL);
  DyLib result = {(u64)so};
  return result;
}

void lib_close(DyLib lib) {
  void *so = (void *)lib.handle;
  dlclose(so);
}

VoidProc *lib_load_proc(DyLib lib, Str8 name) {
  void *so = (void *)lib.handle;
  VoidProc *proc = (VoidProc *)dlsym(so, (char *)name.str);
  return proc;
}
