#ifndef _H_ARENA
#define _H_ARENA

#include "bace/base.h"

#define ARENA_HEADER_SIZE 128

typedef u64 ArenaFlags;
enum {
  ArenaFlag_NoChain = (1 << 0),
  ArenaFlag_LargePages = (1 << 1),
};

typedef struct {
  ArenaFlags flags;
  u64 reserve_size;
  u64 commit_size;
  void *optional_backing_buffer;
  char *allocation_site_file;
  int allocation_site_line;
  char *name;
} ArenaParams;

typedef struct Arena Arena;
struct Arena {
  Arena *prev;
  Arena *current;
  ArenaFlags flags;

  u64 cmt_size;
  u64 res_size;
  u64 base_pos;

  u64 pos;
  u64 cmt;
  u64 res;

  char *allocation_site_file;
  int allocation_site_line;
  char *name;
};

static_assert(sizeof(Arena) <= ARENA_HEADER_SIZE, "arena header size check");

typedef struct {
  Arena *arena;
  u64 pos;
} Temp;

global u64 g_arena_default_reserve_size = MB(64);
global u64 g_arena_default_commit_size = KB(64);
global ArenaFlags g_arena_default_flags = 0;

// creation/destruction
Arena *arena_alloc_(ArenaParams *params);
#define arena_alloc(...)                                                       \
  arena_alloc_(&(ArenaParams){.reserve_size = g_arena_default_reserve_size,    \
                              .commit_size = g_arena_default_commit_size,      \
                              .flags = g_arena_default_flags,                  \
                              .allocation_site_file = __FILE__,                \
                              .allocation_site_line = __LINE__,                \
                              __VA_ARGS__})
void arena_release(Arena *arena);

// core functions
void *arena_push(Arena *arena, u64 size, u64 align, bool zero);
u64 arena_pos(Arena *arena);
void arena_pop_to(Arena *arena, u64 pos);

void arena_clear(Arena *arena);
void arena_pop(Arena *arena, u64 amt);

Temp temp_begin(Arena *arena);
void temp_end(Temp temp);

#define push_array_no_zero_aligned(a, T, c, align)                             \
  (T *)arena_push((a), sizeof(T) * (c), (align), (false))
#define push_array_aligned(a, T, c, align)                                     \
  (T *)arena_push((a), sizeof(T) * (c), (align), (true))
#define push_array_no_zero(a, T, c)                                            \
  push_array_no_zero_aligned(a, T, c, max(8, alignof(T)))
#define push_array(a, T, c) push_array_aligned(a, T, c, max(8, alignof(T)))

#endif // !_H_ARENA
