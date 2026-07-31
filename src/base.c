#include <string.h>

#include "bace/base.h"

u64 wrapped_write(u8 *ring_base, u64 ring_size, u64 ring_pos, void *src_data,
                  u64 src_data_size) {
  assert(src_data_size <= ring_size);
  u64 ring_off = ring_pos % ring_size;
  u64 bytes_before_split = ring_size - ring_off;
  u64 pre_split_bytes = min(bytes_before_split, src_data_size);
  u64 pst_split_bytes = src_data_size - pre_split_bytes;
  void *pre_split_data = src_data;
  void *pst_split_data = ((u8 *)src_data + pre_split_bytes);
  memmove(ring_base + ring_off, pre_split_data, pre_split_bytes);
  memmove(ring_base + 0, pst_split_data, pst_split_bytes);
  return src_data_size;
}

u64 wrapped_read(u8 *ring_base, u64 ring_size, u64 ring_pos, void *dst_data,
                 u64 read_size) {
  assert(read_size <= ring_size);
  u64 ring_off = ring_pos % ring_size;
  u64 bytes_before_split = ring_size - ring_off;
  u64 pre_split_bytes = min(bytes_before_split, read_size);
  u64 pst_split_bytes = read_size - pre_split_bytes;
  memmove(dst_data, ring_base + ring_off, pre_split_bytes);
  memmove((u8 *)dst_data + pre_split_bytes, ring_base + 0, pst_split_bytes);
  return read_size;
}
