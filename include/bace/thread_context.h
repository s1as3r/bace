// thread_context.h
//
// provides a small block of thread-local state (`TCTX`).
// primarily a pair of "scratch" arenas usable for short-lived, temporary allocations
// without callers having to pass an arena down through every call
//
// also proviplus a thread-launching helper that ensures
// every spawned thread gets its own `TCTX` set up before running user code.
//
// exactly one `TCTX` must be allocated and selected (via `tctx_select`) per
// thread before any code on that thread uses `scratch_begin`/`scratch_end` or
// otherwise calls `tctx_selected`.
//
// reference: raddebugger, mr4th

#ifndef _H_THREAD_CONTEXT
#define _H_THREAD_CONTEXT

#include <threads.h> // IWYU pragma: export

#include "base.h"
#include "arena.h"

// thread_local context block.
// one instance exists per thread that has called `tctx_select`.
typedef struct TCTX TCTX;
struct TCTX {
  // a fixed pool of scratch arenas used by `tctx_get_scratch` to hand
  // out temporary arenas that avoid colliding with arenas the caller is
  // already using.
  Arena *arenas[2];

  // stuff not used for now
  u8 thread_name[32];
  u64 thread_name_size;

  char *file_name;
  u64 line_number;
};

// allocates and initializes a new `TCTX`.
// typically called once per thread, near thread startup.
TCTX *tctx_alloc(void);

// releases a `TCTX` previously returned by `tctx_alloc`.
// must not be the currently-selected context of any thread when
// released.
void tctx_release(TCTX *tctx);

// makes `tctx` the thread-local selected context for the calling thread.
// must be called once per thread before using `tctx_selected`,
// `scratch_begin`, or `scratch_end`.
void tctx_select(TCTX *tctx);

// returns the calling thread's currently-selected `TCTX`.
TCTX *tctx_selected(void);

// returns one of the calling thread's scratch arenas that
// does not appear in the `conflicts` array of `count` arena pointers,
// or `null` if every one of the thread's scratch arenas is present in
// `conflicts`.
Arena *tctx_get_scratch(Arena **conflicts, u64 count);

// begins a `Temp` region on a thread-local scratch arena that
// avoids the given `conflicts` list.
// usage:
//   Temp scratch = scratch_begin(0, 0);
//   // ... use scratch.arena for temporary allocations ...
//   scratch_end(scratch);
// or, when the caller already holds another arena that must not be
// reused as scratch:
//   Temp scratch = scratch_begin(&other_arena, 1);
#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))

// ends a scratch region started with `scratch_begin`, rolling the
// underlying scratch arena back to its prior position.
#define scratch_end(scratch) temp_end(scratch);

// launching threads with proper tctx
//
// a minimal thread pool wraps C11 `thrd_create`-style launches so
// that every spawned thread automatically gets a `TCTX` allocated and
// selected before its user function runs, and released after it returns.
// uses C11 threads to keep the implementation portable and simple.

// node representing one in-flight or pooled-but-idle thread launch request.
typedef struct ThreadEntity ThreadEntity;
struct ThreadEntity {
  ThreadEntity *next; // free-list link when unused
  thrd_start_t fn;
  void *data;
};

// a simple free-list pool of `ThreadEntity` nodes, backed by `arena` for
// allocation and guarded by `mutex` for concurrent access from multiple
// threads launching/completing threads simultaneously.
typedef struct ThreadEntityPool {
  Arena *arena;
  ThreadEntity *free_list;
  mtx_t mutex;
} ThreadEntityPool;

// the C11 thread entry point installed for every thread launched
// via `thread_launch_with_ctx`.
// `ptr` is the `ThreadEntity` describing the real user function/data to run.
// sets up a `TCTX`, invokes the wrapped `fn(data)`, then tears the `TCTX` down
// before returning.
// not intended to be called directly by user code pass your function to
// `thread_launch_with_ctx` instead.
i32 thread_entry_point(void *ptr);

// launches a new os thread running `fn(data)`, ensuring it has a valid
// `TCTX` available for its entire lifetime via `thread_entry_point`.
// `thread` receives the resulting `thrd_t` handle (as with `thrd_create`) and
// should be joined/detached by the caller using
// standard C11 threads API.
// returns the thrd_create-style status code (`thrd_success` on success).
i32 thread_launch_with_ctx(thrd_t *thread, thrd_start_t fn, void *data);

#endif // !_H_THREAD_CONTEXT
