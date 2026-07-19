#include "bace/thread_context.h"
#include "bace/arena.h"
#include <threads.h>

thread_local TCTX *tctx_thread_local;

global once_flag g_thread_entity_pool_init_flag = ONCE_FLAG_INIT;
global ThreadEntityPool g_thread_entity_pool;

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

void thread_entity_pool_init(void) {
  // yes this is leaked ig
  g_thread_entity_pool.arena = arena_alloc();
  g_thread_entity_pool.free_list = 0;
  mtx_init(&g_thread_entity_pool.mutex, mtx_plain);
}

ThreadEntity *thread_entity_alloc(void) {
  call_once(&g_thread_entity_pool_init_flag, thread_entity_pool_init);

  mtx_lock(&g_thread_entity_pool.mutex);
  ThreadEntity *entity = g_thread_entity_pool.free_list;
  if (entity) {
    sll_stack_pop(g_thread_entity_pool.free_list);
  } else {
    entity = push_array_no_zero(g_thread_entity_pool.arena, ThreadEntity, 1);
  }
  mtx_unlock(&g_thread_entity_pool.mutex);
  return entity;
}

void thread_entity_release(ThreadEntity *entity) {
  mtx_lock(&g_thread_entity_pool.mutex);
  sll_stack_push(g_thread_entity_pool.free_list, entity);
  mtx_unlock(&g_thread_entity_pool.mutex);
}

i32 thread_entry_point(void *ptr) {
  ThreadEntity *entity = (ThreadEntity *)ptr;

  thrd_start_t fn = entity->fn;
  void *data = entity->data;
  thread_entity_release(entity);

  TCTX *tctx = tctx_alloc();
  tctx_select(tctx);

  i32 result = fn(data);
  tctx_release(tctx);
  return result;
}

i32 thread_launch_with_ctx(thrd_t *thread, thrd_start_t fn, void *data) {
  ThreadEntity *entity = thread_entity_alloc();
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
