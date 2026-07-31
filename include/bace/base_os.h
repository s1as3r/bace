// base_os.h
//
// platform-abstraction layer: declarations for os-provided facilities that every
// supported OS backend (`linux/base.h`, `win32/base.h`, ...) must implement.
//
// references: mr4h, raddebugger

#ifndef _H_BASE_OS
#define _H_BASE_OS

#include "base.h"
#include "strings.h"

// system information

// info about the machine the process is running on.
// call `get_system_info` to access them rather than querying the os.
typedef struct SystemInfo {
  u32 logical_processor_count;
  // os virtual-memory page size, in bytes.
  u64 page_size;
  // size of a large/huge page, in bytes, if the os and hardware support them.
  u64 large_page_size;
  // minimum granularity/alignment at which the os accepts virtual memory
  // reservations
  u64 allocation_granularity;
  Str8 machine_name;
} SystemInfo;

// returns a pointer to the process-wide `SystemInfo`.
// the returned pointer is valid for the lifetime of the process.
// must not be called before `bace_os_state_init`
SystemInfo *get_system_info(void);

// performs initialization of os-layer state.
// populates `SystemInfo`, initializes any platform handles needed by the
// memory/dylib functions.
// must be called once, early in program startup, before using any other function in
// the bace library.
void bace_os_state_init(void);

// memory
//
// low-level virtual-memory primitives used to back the arena allocator.

// reserves `size` bytes of virtual address space without committing
// physical backing.
// returns a pointer to the reserved region, or null on failure.
void *reserve_memory(u64 size);

// commits `size` bytes starting at `ptr`, which must lie within a region
// previously returned by `reserve_memory` and not yet released. `ptr` and
// `size` should be aligned to the platform page size.
// returns `true` on success.
bool commit_memory(void *ptr, u64 size);

// decommits `size` bytes starting at `ptr`, releasing their physical backing
// while keeping the address range reserved.
void decommit_memory(void *ptr, u64 size);

// releases `size` bytes of address space starting at `ptr` back to the os.
void release_memory(void *ptr, u64 size);

// like `reserve_memory`, but requests the reservation using the platform's
// large/huge page mechanism.
// returns `null` on failure.
void *reserve_memory_large(u64 size);

// commits memory within a large-page reservation made via `reserve_memory_large`.
// returns `true` on success.
bool commit_memory_large(void *ptr, u64 size);

// dynamic libraries

// handle to a loaded dynamic/shared library.
typedef struct DyLib {
  u64 handle;
} DyLib;

// loads the dynamic library at `path`.
// returns a `DyLib` with `handle == 0` on failure.
DyLib lib_open(Str8 path);

// unloads a library previously returned by `lib_open`.
// any function pointers obtained from it via lib_load_proc become
// invalid after this call.
void lib_close(DyLib lib);

// looks up the exported symbol `name` in `lib` and returns it as a `VoidProc *`.
// the caller is responsible for casting to the correct function
// signature before calling it.
VoidProc *lib_load_proc(DyLib lib, Str8 name);

#endif // !_H_BASE_OS
