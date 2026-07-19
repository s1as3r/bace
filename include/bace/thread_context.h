#ifndef _H_THREAD_CONTEXT
#define _H_THREAD_CONTEXT

#include <threads.h> // IWYU pragma: export

#include "base.h"
#include "arena.h"

typedef struct TCTX TCTX;
struct TCTX {
  // scratch arenas
  Arena *arenas[2];

  u8 thread_name[32];
  u64 thread_name_size;

  char *file_name;
  u64 line_number;
};

TCTX *tctx_alloc(void);
void tctx_release(TCTX *tctx);
void tctx_select(TCTX *tctx);
TCTX *tctx_selected(void);

Arena *tctx_get_scratch(Arena **conflicts, u64 count);
#define scratch_begin(conflicts, count) temp_begin(tctx_get_scratch((conflicts), (count)))
#define scratch_end(scratch) temp_end(scratch);

// launching threads with proper tctx
// using c11 threads to keep things simple
typedef struct ThreadEntity ThreadEntity;
struct ThreadEntity {
  ThreadEntity *next; // free-list link when unused
  thrd_start_t fn;
  void *data;
};

typedef struct ThreadEntityPool {
  Arena *arena;
  ThreadEntity *free_list;
  mtx_t mutex;
} ThreadEntityPool;

i32 thread_entry_point(void *ptr);
i32 thread_launch_with_ctx(thrd_t *thread, thrd_start_t fn, void *data);

#endif // !_H_THREAD_CONTEXT
