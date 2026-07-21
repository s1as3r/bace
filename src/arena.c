#include <string.h>

#include "bace/base.h"
#include "bace/arena.h"
#include "bace/base_os.h"

Arena *arena_alloc_(ArenaParams *params) {
  u64 res_size = params->reserve_size;
  u64 cmt_size = params->commit_size;

  // reserve initial block
  void *base = params->optional_backing_buffer;
  if (base == 0) {
    SystemInfo sys_info = *get_system_info();
    if (params->flags & ArenaFlag_LargePages) {
      res_size = align_pow_2(res_size, sys_info.large_page_size);
      cmt_size = align_pow_2(cmt_size, sys_info.large_page_size);
      base = reserve_memory_large(res_size);
      commit_memory_large(base, cmt_size);
    } else {
      res_size = align_pow_2(res_size, sys_info.page_size);
      cmt_size = align_pow_2(cmt_size, sys_info.page_size);
      base = reserve_memory(res_size);
      commit_memory(base, cmt_size);
    }
  }

  Arena *arena = base;
  arena->current = arena;
  arena->flags = params->flags;
  arena->cmt_size = params->commit_size;
  arena->res_size = params->reserve_size;
  arena->base_pos = 0;
  arena->pos = ARENA_HEADER_SIZE;
  arena->cmt = cmt_size;
  arena->res = res_size;
  arena->allocation_site_file = params->allocation_site_file;
  arena->allocation_site_line = params->allocation_site_line;
  arena->name = params->name;

  return arena;
}

void arena_release(Arena *arena) {
  for (Arena *n = arena->current, *prev = 0; n != 0; n = prev) {
    prev = n->prev;
    release_memory(n, n->res);
  }
}

void *arena_push(Arena *arena, u64 size, u64 align, bool zero) {
  Arena *curr = arena->current;
  u64 pos_pre = align_pow_2(curr->pos, align);
  u64 pos_pst = pos_pre + size;

  // chain, if needed
  if ((curr->res < pos_pst) && !(arena->flags & ArenaFlag_NoChain)) {
    u64 res_size = curr->res_size;
    u64 cmt_size = curr->cmt_size;
    if (size + ARENA_HEADER_SIZE > res_size) {
      res_size = align_pow_2(size + ARENA_HEADER_SIZE, align);
      cmt_size = align_pow_2(size + ARENA_HEADER_SIZE, align);
    }
    Arena *new_block = arena_alloc(.reserve_size = res_size, .commit_size = cmt_size,
                                   .flags = curr->flags,
                                   .allocation_site_file = curr->allocation_site_file,
                                   .allocation_site_line = curr->allocation_site_line);

    new_block->base_pos = curr->base_pos + curr->res;
    sll_stack_push_n(arena->current, new_block, prev);

    curr = new_block;
    pos_pre = align_pow_2(curr->pos, align);
    pos_pst = pos_pre + size;
  }

  // commit new pages if needed
  if (curr->cmt < pos_pst) {
    u64 cmt_pst_aligned = pos_pst + curr->cmt_size - 1;
    cmt_pst_aligned -= cmt_pst_aligned % curr->cmt_size;
    u64 cmt_pst_clamped = min(cmt_pst_aligned, curr->res);
    u64 cmt_size = cmt_pst_clamped - curr->cmt;
    u8 *cmt_ptr = (u8 *)curr + curr->cmt;
    if (curr->flags & ArenaFlag_LargePages) {
      commit_memory_large(cmt_ptr, cmt_size);
    } else {
      commit_memory(cmt_ptr, cmt_size);
    }
    curr->cmt = cmt_pst_clamped;
  }

  // push onto curr block
  void *result = 0;
  if (curr->cmt >= pos_pst) {
    result = (u8 *)curr + pos_pre;
    curr->pos = pos_pst;
    if (zero) {
      memset(result, 0, size);
    }
  }

  return result;
}

u64 arena_pos(Arena *arena) {
  Arena *curr = arena->current;
  u64 pos = curr->base_pos + curr->pos;
  return pos;
}

void arena_pop_to(Arena *arena, u64 pos) {
  u64 big_pos = max(ARENA_HEADER_SIZE, pos);
  Arena *curr = arena->current;

  for (Arena *prev = 0; curr->base_pos >= big_pos; curr = prev) {
    prev = curr->prev;
    release_memory(curr, curr->res);
  }
  arena->current = curr;

  u64 new_pos = big_pos - curr->base_pos;
  assert(new_pos <= curr->pos);
  curr->pos = new_pos;
}

void arena_clear(Arena *arena) {
  arena_pop_to(arena, 0);
}

void arena_pop(Arena *arena, u64 amt) {
  u64 pos = arena_pos(arena);
  amt = clamp(amt, 0, pos);
  u64 pos_new = pos - amt;
  arena_pop_to(arena, pos_new);
}

Temp temp_begin(Arena *arena) {
  u64 pos = arena_pos(arena);
  Temp temp = {arena, pos};
  return temp;
}

void temp_end(Temp temp) {
  arena_pop_to(temp.arena, temp.pos);
}
