// arena.h
//
// arena (linear/bump) allocator.
//
// an arena hands out memory by simply bumping a `pos` cursor forward inside
// a large virtual-memory reservation, committing physical pages on demand.
// individual allocations are never freed one at a time, instead the whole
// arena (or a range within it, via `arena_pop_to` / `Temp`) is reclaimed at
// once.
//
// large arenas are implemented as a chain of arena blocks: when the
// current block runs out of reserved space, a new block is allocated and
// linked in via `current`/`prev`, unless `ArenaFlag_NoChain` is set.
// `pos` is a logical, arena-wide position, while `cmt`/
// `res` track the committed/reserved bytes of the current block only.
//
// thread safety: an individual Arena is *NOT* thread-safe.
// it must not be pushed to concurrently from multiple threads without external
// synchronization.
// see `thread_context.h` for per-thread scratch arenas.
//
// references: raddebugger, mr4th

#ifndef _H_ARENA
#define _H_ARENA

#include "bace/base.h"

// size, in bytes, reserved at the front of every arena block for the
// Arena header itself. the header is placed inline at the start of the block's
// reserved memory , ahead of the region handed out to callers.
#define ARENA_HEADER_SIZE 128

typedef u64 ArenaFlags;
enum {
  // disallow chaining: once the block's reserved space is exhausted,
  // `arena_push` fails instead of allocating a new chained block.
  // useful for arenas that must live in a single fixed-size allocation
  // (e.g. backed by `optional_backing_buffer`).
  ArenaFlag_NoChain = (1 << 0),
  // request the os large-page mechanism for this arena's reservation,
  // trading flexibility (must be sized/aligned to the large page size) for
  // reduced TLB pressure.
  ArenaFlag_LargePages = (1 << 1),
};

// params for creating a new arena via `arena_alloc()`.
// all fields are optional in the sense that arena_alloc() fills in project-wide
// defaults (see `ARENA_DEFAULT_*` below) for anything not explicitly overridden.
// pass named-initializer overrides, e.g. `arena_alloc(.reserve_size = MB(1))`.
typedef struct ArenaParams {
  ArenaFlags flags;
  // total virtual address space to reserve up front.
  // this is an upper bound on how large this single arena block can grow to
  // before a new chained block is created (or allocation fails, with `NoChain` set).
  u64 reserve_size;
  // how many bytes to commit initially, and the
  // granularity by which additional commits grow the block as `pos`
  // advances past the currently-committed region.
  u64 commit_size;
  // if non-null, use this caller-provided buffer as the arena's backing
  // storage instead of reserving new virtual memory.
  // typically paired with `ArenaFlag_NoChain` since the buffer has a fixed size.
  // the buffer must outlive the arena.
  void *optional_backing_buffer;
  // debug/diagnostic info identifying where this arena was created.
  // normally filled in automatically by the arena_alloc() macro via __FILE__/__LINE__.
  char *allocation_site_file;
  int allocation_site_line;
  // optional name for debugging/tooling.
  char *name;
} ArenaParams;

// a single block in an arena chain.
// `current` always points to the most recently allocated block in the chain
// (i.e. the one currently receiving pushes).
// on the first/only block, `current == self`.
// `prev` links back toward the chain's origin.
// only the values on the `current` block are authoritative for `pos`/`cmt`/`res`
// bookkeeping of the whole chain.
// non-current blocks retain the state they had when they were superseded.
typedef struct Arena Arena;
struct Arena {
  Arena *prev;
  Arena *current;
  ArenaFlags flags;

  // commit/reserve granularity and total reservation size for this block.
  // inherited by any blocks chained after it.
  u64 cmt_size;
  u64 res_size;
  // logical position, in bytes, at which this block's local address range begins
  // withing the arena-wide (whole chain) space.
  // used to translate a chain-wide `pos` into an offset within a specific block.
  u64 base_pos;

  // arena-wide cursor: the next allocation will start out at this logical position
  u64 pos;
  // bytes committed in this block so far
  u64 cmt;
  // bytes reserved for this block
  u64 res;

  char *allocation_site_file;
  int allocation_site_line;
  char *name;
};

static_assert(sizeof(Arena) <= ARENA_HEADER_SIZE, "arena header size check");

// a saved position within a specific arena, used to roll the arena back to
// an earlier state (used for scratch arenas, see `thread_context.h`).
typedef struct Temp {
  Arena *arena;
  u64 pos;
} Temp;

#define ARENA_DEFAULT_RESERVE_SIZE MB(64)
#define ARENA_DEFAULT_COMMIT_SIZE KB(64)
#define ARENA_DEFAULT_FLAGS 0

// arena creation/destruction

// underlying implementation for arena_alloc().
// prefer using the macro in normal call sites so allocation-site info
// and defaults are filled in automatically.
// `params` must be non-null.
// returns a new Arena whose header lives at the start of its own backing memory.
Arena *arena_alloc_(ArenaParams *params);

// allocates a new Arena, using defaults for any ArenaParams field not
// explicitly supplied.
// usage:
//   Arena *a = arena_alloc();
//   Arena *b = arena_alloc(.reserve_size = MB(4), .name = "my arena");
#define arena_alloc(...)                                                                 \
  arena_alloc_(&(ArenaParams){.reserve_size = ARENA_DEFAULT_RESERVE_SIZE,                \
                              .commit_size = ARENA_DEFAULT_COMMIT_SIZE,                  \
                              .flags = ARENA_DEFAULT_FLAGS,                              \
                              .allocation_site_file = __FILE__,                          \
                              .allocation_site_line = __LINE__,                          \
                              __VA_ARGS__})

// releases an entire arena chain back to the os.
// `arena` must be the same pointer originally returned by `arena_alloc`,
// passing an arbitrary chained block will not release the chain correctly.
//
// CAUTION: avoid calling `arena_release` on an arena that was allocated using an
// `optional_backing_buffer`.
void arena_release(Arena *arena);

// core functions

// bumps the arena's cursor forward and returns a pointer to a fresh
// `size`-byte region, aligned to `align` (which must be a power of two).
// if `zero` is `true`, the returned memory is zero-initialized, otherwise
// its contents are unspecified.
// transparently commits additional pages and/or chains a new block as
// needed to satisfy the request (unless ArenaFlag_NoChain is set on the arena,
// in which case exceeding the reservation is a failure).
void *arena_push(Arena *arena, u64 size, u64 align, bool zero);

// returns the arena's current chain-wide logical position (i.e. total
// bytes allocated so far across the whole chain).
// useful in combination with `arena_pop_to` to save/restore a rollback
// point manually (see also `temp_begin`/`temp_end` for the common case).
u64 arena_pos(Arena *arena);

// rolls the arena's cursor back to a previously observed `pos`, invalidating
// every allocation made after that point. does not necessarily decommit memory or
// release chained blocks immediately.
// `pos` must be <= the arena's current position and must have been obtained from
// this same arena chain.
void arena_pop_to(Arena *arena, u64 pos);

// resets the arena to empty invalidating all prior allocations from it.
// the arena and its committed memory remain valid and reusable for further pushes.
void arena_clear(Arena *arena);

// rolls the arena's cursor back by `amt` bytes from its current position.
void arena_pop(Arena *arena, u64 amt);

// begins a scoped, rollback-able region on `arena`: saves the current
// position so it can later be restored via temp_end.
// `Temp` regions may be nested as long as they are ended in strict LIFO order
// relative to other Temp regions and any other pushes made on the
// same arena during the scope.
Temp temp_begin(Arena *arena);

// ends a scoped region started with `temp_begin`, rolling `temp.arena` back
// to the position it had when `temp_begin` was called.
// any allocations made on that arena since the matching temp_begin become invalid.
void temp_end(Temp temp);

// typed array allocation helpers
//
// all of these push `sizeof(T) * c` bytes out of arena `a`, returning a `T *`.
// the `_no_zero` variants leave the memory uninitialized and the plain variants zero it.
// `_aligned` variants let the caller specify an explicit alignment
// (must be a power of two).
// the non-`_aligned` variants default to `max(8, alignof(T))`.
#define push_array_no_zero_aligned(a, T, c, align)                                       \
  (T *)arena_push((a), sizeof(T) * (c), (align), (false))
#define push_array_aligned(a, T, c, align)                                               \
  (T *)arena_push((a), sizeof(T) * (c), (align), (true))
#define push_array_no_zero(a, T, c)                                                      \
  push_array_no_zero_aligned(a, T, c, max(8, alignof(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, max(8, alignof(T)))

#endif // !_H_ARENA
