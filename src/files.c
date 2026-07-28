#include <string.h>
#include "bace/files.h"
#include "bace/thread_context.h"

File file_zero(void) {
  File f = {0};
  return f;
}

bool file_match(File a, File b) {
  bool result = memcmp(&a, &b, sizeof(a)) == 0;
  return result;
}

Str8 data_from_file_path(Arena *arena, Str8 path) {
  File file = file_open(AccessFlag_Read | AccessFlag_ShareRead, path);
  FileProperties props = properties_from_file(file);
  Str8 data = string_from_file_range(arena, file, 0, props.size);
  file_close(file);
  return data;
}

bool write_data_to_file_path(Str8 path, Str8 data) {
  bool good = 0;
  File file = file_open(AccessFlag_Write, path);
  if (!file_match(file, file_zero())) {
    u64 bytes_written = file_write(file, 0, data.size, data.str);
    good = (bytes_written == data.size);
    file_close(file);
  }
  return good;
}

bool write_data_list_to_file_path(Str8 path, Str8List list) {
  bool good = 0;
  File file = file_open(AccessFlag_Write, path);
  if (!file_match(file, file_zero())) {
    Temp scratch = scratch_begin(0, 0);
    u64 write_buffer_size = KB(64);
    u8 *write_buffer = push_array_no_zero(scratch.arena, u8, write_buffer_size);
    u64 write_buffer_write_pos = 0;
    u64 write_buffer_read_pos = 0;
    u64 file_off = 0;
    {
      for (Str8Node *n = list.first; n != 0; n = n->next) {
        for (u64 n_off = 0; n_off < n->str.size;) {
          u64 write_buffer_unconsumed_size =
              (write_buffer_write_pos - write_buffer_read_pos);
          u64 write_buffer_available_size =
              (write_buffer_size - write_buffer_unconsumed_size);
          if (write_buffer_available_size == 0) {
            u64 file_write_size =
                file_write(file, file_off, file_off + write_buffer_size, write_buffer);
            if (file_write_size != write_buffer_size) {
              goto dbl_break;
            }
            file_off += write_buffer_size;
            write_buffer_read_pos += write_buffer_size;
          } else {
            u64 bytes_to_copy = min(write_buffer_available_size, n->str.size - n_off);
            write_buffer_write_pos +=
                wrapped_write(write_buffer, write_buffer_size, write_buffer_write_pos,
                              n->str.str + n_off, bytes_to_copy);
            n_off += bytes_to_copy;
          }
        }
      }
      if (write_buffer_write_pos > write_buffer_read_pos) {
        u64 file_write_size = file_write(
            file, file_off, file_off + (write_buffer_write_pos - write_buffer_read_pos),
            write_buffer);
        file_off += file_write_size;
      }
    }
  dbl_break:;
    good = (file_off == list.total_size);
    file_close(file);
    scratch_end(scratch);
  }
  return good;
}

bool append_data_to_file_path(Str8 path, Str8 data) {
  bool good = 0;
  if (data.size != 0) {
    File file = file_open(AccessFlag_Write | AccessFlag_Append, path);
    if (!file_match(file, file_zero())) {
      u64 pos = properties_from_file(file).size;
      u64 bytes_written = file_write(file, pos, pos + data.size, data.str);
      good = (bytes_written == data.size);
      file_close(file);
    }
  }
  return good;
}

FileID id_from_file_path(Str8 path) {
  File file = file_open(AccessFlag_Read | AccessFlag_ShareRead, path);
  FileID id = id_from_file(file);
  file_close(file);
  return id;
}

i64 file_id_compare(FileID a, FileID b) {
  i64 cmp = memcmp((void *)&a.v[0], (void *)&b.v[0], sizeof(a.v));
  return cmp;
}

Str8 string_from_file_range(Arena *arena, File file, u64 mn, u64 mx) {
  u64 pre_pos = arena_pos(arena);
  Str8 result;
  result.size = (mx >= mn) ? mx - mn : 0;
  result.str = push_array_no_zero(arena, u8, result.size);
  u64 actual_read_size = file_read(file, mn, mx, result.str);
  if (actual_read_size < result.size) {
    arena_pop_to(arena, pre_pos + actual_read_size);
    result.size = actual_read_size;
  }
  return result;
}

Str8 file_read_cstring(Arena *arena, File file, u64 off) {
  Temp scratch = scratch_begin(&arena, 1);
  Str8List block_list = {0};
  for (u64 cursor = off, stride = 256;; cursor += stride) {
    u8 *raw_block = push_array_no_zero(scratch.arena, u8, stride);
    u64 read_size = file_read(file, cursor, cursor + stride, raw_block);
    u8 *ptr = raw_block;
    for (; ptr < (raw_block + read_size); ptr += 1);
    u64 size = (u64)(ptr - (u8 *)raw_block);
    Str8 block = str8(raw_block, size);
    str8_list_push(scratch.arena, &block_list, block);
    if (read_size != stride ||
        (block.size + 1 <= read_size && block.str[block.size] == 0)) {
      break;
    }
  }
  Str8 result = str8_list_join(arena, &block_list, 0);
  scratch_end(scratch);
  return result;
}
