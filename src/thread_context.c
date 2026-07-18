#include "bace/thread_context.h"

thread_local TCTX *tctx_thread_local;

TCTX *tctx_alloc(void) {
  Arena *arena_0 = arena_alloc();
  Arena *arena_1 = arena_alloc();

  TCTX *tctx = push_array(arena_0, TCTX, 1);
  tctx->arenas[0] = arena_0;
  tctx->arenas[1] = arena_1;

  return tctx;
}

void tctx_release(TCTX *tctx) {
  arena_release(tctx->arenas[1]);
  arena_release(tctx->arenas[0]);
}

void tctx_select(TCTX *tctx) {
  tctx_thread_local = tctx;
}

TCTX *tctx_selected(void) {
  return tctx_thread_local;
}

i32 thread_entry_point(void *ptr) {
  ThreadEntity *entity = (ThreadEntity *)ptr;

  TCTX *tctx = tctx_alloc();
  tctx_select(tctx);

  i32 result = entity->fn(entity->data);
  tctx_release(tctx);
  return result;
}

i32 thread_launch_with_ctx(Arena *arena, thrd_t *thread, thrd_start_t fn, void *data) {
  ThreadEntity *entity = push_array_no_zero(arena, ThreadEntity, 1);
  entity->fn = fn;
  entity->data = data;

  return thrd_create(thread, thread_entry_point, (void *)entity);
}

Arena *tctx_get_scratch(Arena **conflicts, u64 count) {
  TCTX *tctx = tctx_selected();
  Arena *result = 0;
  Arena **arena_ptr = tctx->arenas;

  for (u64 i = 0; i < array_count(tctx->arenas); i += 1, arena_ptr += 1) {
    Arena **conflict_ptr = conflicts;
    bool has_conflict = false;
    for (u64 j = 0; j < count; j += 1, conflict_ptr += 1) {
      if (*arena_ptr == *conflict_ptr) {
        has_conflict = true;
        break;
      }
    }
    if (!has_conflict) {
      result = *arena_ptr;
      break;
    }
  }

  return result;
}
