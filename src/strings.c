#include "bace/base.h"
#include "bace/strings.h"
#include "bace/arena.h"

#include <string.h>
#include <stdio.h>

// clang-format off
global u8 integer_symbol_reverse[128] =
{
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
};
// clang-format on

// character classification
bool char_is_space(u8 c) {
  return (c == ' ' || c == '\n' || c == '\t' || c == '\r' || c == '\f' || c == '\v');
}

bool char_is_upper(u8 c) {
  return ('A' <= c && c <= 'Z');
}

bool char_is_lower(u8 c) {
  return ('a' <= c && c <= 'z');
}

bool char_is_alpha(u8 c) {
  return (char_is_lower(c) || char_is_lower(c));
}

bool char_is_digit(u8 c, u32 base) {
  bool result = false;
  if (0 < base && base <= 16) {
    u8 val = integer_symbol_reverse[c];
    if (val < base) {
      result = true;
    }
  }
  return result;
}

bool char_is_slash(u8 c) {
  return (c == '/' || c == '\\');
}

// char conversion
u8 char_to_lower(u8 c) {
  if (char_is_upper(c)) {
    c += ('a' - 'A');
  }
  return c;
}

u8 char_to_upper(u8 c) {
  if (char_is_lower(c)) {
    c += (u8)('A' - 'a');
  }
  return c;
}

u8 char_simplify_slash(u8 c) {
  if (char_is_slash(c)) {
    c = '/';
  }
  return c;
}

u64 cstring8_length(const u8 *str) {
  u64 len = 0;
  if (!str) {
    return len;
  }
  u8 *p = (u8 *)str;
  for (; *p != 0; p += 1);
  len = (u64)(p - str);
  return len;
}

// constructors
Str8 str8(u8 *str, u64 size) {
  Str8 result = {str, size};
  return result;
}

Str8 str8_zero(void) {
  Str8 result = {0};
  return result;
}

Str8 str8_cstring(const char *c) {
  Str8 result = {(u8 *)c, cstring8_length((const u8 *)c)};
  return result;
}

Str8 str8_range(u8 *first, u8 *one_past_last) {
  Str8 result = {first, (u64)(one_past_last - first)};
  return result;
}

Str8 str8_to_upper(Arena *arena, Str8 string) {
  string = str8_copy(arena, string);
  for (u64 idx = 0; idx < string.size; idx += 1) {
    string.str[idx] = char_to_upper(string.str[idx]);
  }
  return string;
}

Str8 str8_to_lower(Arena *arena, Str8 string) {
  string = str8_copy(arena, string);
  for (u64 idx = 0; idx < string.size; idx += 1) {
    string.str[idx] = char_to_lower(string.str[idx]);
  }
  return string;
}

// copying and fomatting
Str8 str8_cat(Arena *arena, Str8 s1, Str8 s2) {
  Str8 str = {
      .size = s1.size + s2.size,
      .str = push_array_no_zero(arena, u8, s1.size + s2.size + 1),
  };
  memmove(str.str, s1.str, s1.size);
  memmove(str.str + s1.size, s2.str, s2.size);
  str.str[str.size] = 0;
  return str;
}

Str8 str8_copy(Arena *arena, Str8 s) {
  Str8 str = {
      .size = s.size,
      .str = push_array_no_zero(arena, u8, s.size + 1),
  };
  memmove(str.str, s.str, s.size);
  str.str[str.size] = 0;
  return str;
}

Str8 str8fv(Arena *arena, const char *fmt, va_list args) {
  va_list args2;
  va_copy(args2, args);

  Str8 result = {0};
  u32 needed_bytes = (u32)vsnprintf(0, 0, fmt, args) + 1;
  result.str = push_array_no_zero(arena, u8, needed_bytes);
  result.size = (u64)vsnprintf((char *)result.str, needed_bytes, fmt, args2);
  result.str[result.size] = 0;

  va_end(args2);
  return result;
}

Str8 str8f(Arena *arena, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  Str8 result = str8fv(arena, fmt, args);
  va_end(args);
  return result;
}

// slicing
Str8 str8_substr(Str8 str, u64 min, u64 max) {
  min = min(min, str.size);
  max = min(max, str.size);
  str.str += min;
  str.size = (max > min) ? (max - min) : 0;
  return str;
}

Str8 str8_prefix(Str8 str, u64 size) {
  str.size = min(size, str.size);
  return str;
}

Str8 str8_skip(Str8 str, u64 amt) {
  amt = min(amt, str.size);
  str.str += amt;
  str.size -= amt;
  return str;
}

Str8 str8_postfix(Str8 str, u64 size) {
  size = min(size, str.size);
  str.str = (str.str + str.size) - size;
  str.size = size;
  return str;
}

Str8 str8_chop(Str8 str, u64 amt) {
  amt = min(amt, str.size);
  str.size -= amt;
  return str;
}

Str8 str8_chop_line(Str8 *str) {
  u64 new_line_pos = str8_find_needle(*str, 0, str8_lit("\n"), 0);
  Str8 line = str8_prefix(*str, new_line_pos);
  if (str8_ends_with(line, str8_lit("\r"), 0)) {
    line = str8_chop(line, 1);
  }
  *str = str8_skip(*str, new_line_pos + 1);
  return line;
}

Str8 str8_skip_chop(Str8 string, bool (*predicate)(u8 c)) {
  u8 *first = string.str;
  u8 *opl = first + string.size;
  for (; first < opl; first += 1) {
    if (!predicate(*first)) {
      break;
    }
  }
  for (; opl > first;) {
    opl -= 1;
    if (!predicate(*opl)) {
      opl += 1;
      break;
    }
  }
  Str8 result = str8_range(first, opl);
  return result;
}
Str8 str8_skip_chop_whitespace(Str8 string) {
  return str8_skip_chop(string, char_is_space);
}

Str8 str8_skip_chop_slashes(Str8 string) {
  return str8_skip_chop(string, char_is_slash);
}

// matching
bool str8_char_match(u8 a, u8 b, StringMatchFlags flags) {
  if (flags & StringMatchFlag_CaseInsensitive) {
    a = char_to_lower(a);
    b = char_to_lower(b);
  }
  if (flags & StringMatchFlag_SlashInsensitive) {
    a = char_simplify_slash(a);
    b = char_simplify_slash(b);
  }
  return a == b;
}

bool str8_match(Str8 a, Str8 b, StringMatchFlags flags) {
  bool result = false;
  if (a.size == b.size && flags == 0) {
    result = (memcmp(a.str, b.str, b.size) == 0);
  } else if (a.size == b.size || (flags & StringMatchFlag_RightSideSloppy)) {
    u64 size = min(a.size, b.size);
    result = true;
    for (u64 i = 0; i < size; i++) {
      if (!str8_char_match(a.str[i], b.str[i], flags)) {
        result = false;
        break;
      }
    }
  }
  return result;
}

bool str8_match_wildcard(Str8 string, Str8 pattern, StringMatchFlags flags) {
  bool matched = false;

  u64 pattern_cursor = 0;
  u64 string_cursor = 0;

  u64 pattern_start = UINT64_MAX;
  u64 string_start = 0;

  for (;;) {
    if (pattern_cursor == pattern.size) {
      if (string_cursor == string.size || (flags & StringMatchFlag_RightSideSloppy)) {
        matched = true;
        break;
      }
    }

    if (string_cursor == string.size) {
      while (pattern_cursor < pattern.size && pattern.str[pattern_cursor] == '*') {
        pattern_cursor += 1;
      }
      matched = (pattern_cursor == pattern.size);
      break;
    }

    if (pattern_cursor < pattern.size && pattern.str[pattern_cursor] == '*') {
      pattern_start = pattern_cursor;
      string_start = string_cursor;
      pattern_cursor += 1;
      continue;
    }

    if (pattern_cursor < pattern.size &&
        (pattern.str[pattern_cursor] == '?' ||
         str8_char_match(string.str[string_cursor], pattern.str[pattern_cursor],
                         flags))) {
      string_cursor += 1;
      pattern_cursor += 1;
      continue;
    }

    if (pattern_start != UINT64_MAX) {
      pattern_cursor = pattern_start + 1;
      string_start += 1;
      string_cursor = string_start;
      continue;
    }

    break;
  }

  return matched;
}

u64 str8_find_needle(Str8 string, u64 start_pos, Str8 needle, StringMatchFlags flags) {
  u8 *p = string.str + start_pos;
  u64 stop_offset = max(string.size + 1, needle.size) - needle.size;
  u8 *stop_p = string.str + stop_offset;
  if (needle.size > 0) {
    u8 *str_opl = string.str + string.size;
    Str8 needle_tail = str8_skip(needle, 1);
    StringMatchFlags adj_flags = flags | StringMatchFlag_RightSideSloppy;
    u8 needle_first_char_adj = needle.str[0];
    if (adj_flags & StringMatchFlag_CaseInsensitive) {
      needle_first_char_adj = char_to_lower(needle_first_char_adj);
    }
    if (adj_flags & StringMatchFlag_SlashInsensitive) {
      needle_first_char_adj = char_simplify_slash(needle_first_char_adj);
    }
    for (; p < stop_p; p += 1) {
      u8 haystack_char_adj = *p;
      if (adj_flags & StringMatchFlag_CaseInsensitive) {
        haystack_char_adj = char_to_lower(haystack_char_adj);
      }
      if (adj_flags & StringMatchFlag_SlashInsensitive) {
        haystack_char_adj = char_simplify_slash(haystack_char_adj);
      }
      if (haystack_char_adj == needle_first_char_adj) {
        if (str8_match(str8_range(p + 1, str_opl), needle_tail, adj_flags)) {
          break;
        }
      }
    }
  }
  u64 result = string.size;
  if (p < stop_p) {
    result = (u64)(p - string.str);
  }
  return result;
}

u64 str8_find_needle_reverse(Str8 string, u64 start_pos, Str8 needle,
                             StringMatchFlags flags) {
  u64 result = 0;
  for (i64 i = (i64)(string.size - start_pos - needle.size); i >= 0; i -= 1) {
    Str8 haystack = str8_substr(string, (u64)i, (u64)i + needle.size);
    if (str8_match(haystack, needle, flags)) {
      result = (u64)i + needle.size;
      break;
    }
  }
  return result;
}
bool str8_is_before(Str8 a, Str8 b) {
  bool result = false;
  u64 common_size = min(a.size, b.size);
  for (u64 off = 0; off < common_size; off += 1) {
    if (a.str[off] < b.str[off]) {
      result = true;
      break;
    } else if (a.str[off] > b.str[off]) {
      result = false;
      break;
    } else if (off + 1 == common_size) {
      result = (a.size < b.size);
    }
  }
  return result;
}

// list stuff
Str8Node *str8_list_push_node(Str8List *list, Str8Node *node) {
  sll_queue_push(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += node->str.size;
  return node;
}

Str8Node *str8_list_push_node_set_string(Str8List *list, Str8Node *node, Str8 str) {
  sll_queue_push(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += str.size;
  node->str = str;
  return node;
}

Str8Node *str8_list_push_node_front(Str8List *list, Str8Node *node) {
  sll_queue_push_front(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += node->str.size;
  return node;
}

Str8Node *str8_list_push_node_front_set_string(Str8List *list, Str8Node *node,
                                               Str8 string) {
  sll_queue_push_front(list->first, list->last, node);
  list->node_count += 1;
  list->total_size += string.size;
  node->str = string;
  return node;
}

Str8Node *str8_list_push(Arena *arena, Str8List *list, Str8 str) {
  Str8Node *node = push_array_no_zero(arena, Str8Node, 1);
  return str8_list_push_node_set_string(list, node, str);
}

Str8Node *str8_list_push_front(Arena *arena, Str8List *list, Str8 str) {
  Str8Node *node = push_array_no_zero(arena, Str8Node, 1);
  return str8_list_push_node_front_set_string(list, node, str);
}

void str8_list_concat_in_place(Str8List *list, Str8List *to_push) {
  if (to_push->node_count == 0) {
    return;
  }
  if (list->last) {
    list->node_count += to_push->node_count;
    list->total_size += to_push->total_size;
    list->last->next = to_push->first;
    list->last = to_push->last;
  } else {
    *list = *to_push;
  }
  memset(to_push, 0, sizeof(*to_push));
}

Str8Node *str8_list_pushf(Arena *arena, Str8List *list, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  Str8 str = str8fv(arena, fmt, args);
  Str8Node *node = str8_list_push(arena, list, str);
  va_end(args);
  return node;
}

Str8Node *str8_list_push_frontf(Arena *arena, Str8List *list, char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  Str8 str = str8fv(arena, fmt, args);
  Str8Node *node = str8_list_push(arena, list, str);
  va_end(args);
  return node;
}

Str8Node *str8_list_pop_front(Str8List *list) {
  Str8Node *node = 0;
  if (list->node_count == 0) {
    return node;
  }

  node = list->first;
  assert(list->total_size >= list->first->str.size);
  list->node_count -= 1;
  list->total_size -= list->first->str.size;
  sll_queue_pop(list->first, list->last);
  return node;
}

Str8List str8_list_copy(Arena *arena, Str8List *list) {
  Str8List result = {0};
  for (Str8Node *node = list->first; node != 0; node = node->next) {
    Str8Node *new_node = push_array_no_zero(arena, Str8Node, 1);
    Str8 new_str = str8_copy(arena, node->str);
    str8_list_push_node_set_string(&result, new_node, new_str);
  }
  return result;
}

Str8List str8_list_substr(Arena *arena, Str8List list, u64 min, u64 max) {
  Str8List result = {0};
  Str8Node *n = list.first;

  u64 front_min = 0;
  u64 cursor = 0;
  for (; n != 0; cursor += n->str.size, n = n->next) {
    if (cursor + n->str.size > min) {
      front_min = min - cursor;
      break;
    }
  }

  u64 range_sz = max > min ? max - min : 0;
  if (front_min > 0) {
    u64 front_max = front_min + min(range_sz, n->str.size);
    str8_list_push(arena, &result, str8_substr(n->str, front_min, front_max));
    n = n->next;
  }

  for (; n != 0; n = n->next) {
    if (result.total_size >= range_sz) {
      break;
    }
    u64 copy_max = range_sz - result.total_size;
    u64 copy_sz = min(copy_max, n->str.size);
    str8_list_push(arena, &result, str8_substr(n->str, 0, copy_sz));
  }

  return result;
}

// split & join
Str8List str8_split(Arena *arena, Str8 str, u8 *split_chars, u64 split_char_count,
                    StringSplitFlags flags) {
  Str8List list = {0};
  bool keep_empties = (flags & StringSplitFlag_KeepEmpties);
  u8 *ptr = str.str;
  u8 *opl = str.str + str.size;
  for (; ptr < opl;) {
    u8 *first = ptr;
    for (; ptr < opl; ptr += 1) {
      u8 c = *ptr;
      bool is_split = false;
      for (u64 i = 0; i < split_char_count; i += 1) {
        if (split_chars[i] == c) {
          is_split = true;
          break;
        }
      }
      if (is_split) {
        break;
      }
    }
    Str8 string = str8_range(first, ptr);
    if (keep_empties || string.size > 0) {
      str8_list_push(arena, &list, string);
    }
    ptr += 1;
  }
  return list;
}

Str8List str8_split_by_string_chars(Arena *arena, Str8 str, Str8 split_chars,
                                    StringSplitFlags flags) {
  Str8List list = str8_split(arena, str, split_chars.str, split_chars.size, flags);
  return list;
}

Str8 str8_list_join(Arena *arena, Str8List *list, StringJoin *optional_params) {
  StringJoin join = {0};
  if (optional_params != 0) {
    memmove(&join, optional_params, sizeof(*optional_params));
  }
  u64 sep_count = 0;
  if (list->node_count > 0) {
    sep_count = list->node_count - 1;
  }

  Str8 result;
  result.size =
      join.pre.size + join.post.size + sep_count * join.sep.size + list->total_size;
  u8 *ptr = result.str = push_array_no_zero(arena, u8, result.size + 1);
  memmove(ptr, join.pre.str, join.pre.size);
  ptr += join.pre.size;
  for (Str8Node *node = list->first; node != 0; node = node->next) {
    memmove(ptr, node->str.str, node->str.size);
    ptr += node->str.size;
    if (node->next != 0) {
      memmove(ptr, join.sep.str, join.sep.size);
      ptr += join.sep.size;
    }
  }
  memmove(ptr, join.post.str, join.post.size);
  ptr += join.post.size;
  *ptr = 0;
  return result;
}

// string array function

Str8Array str8_array_zero(void) {
  Str8Array zero = {0};
  return zero;
}

Str8Array str8_array_from_list(Arena *arena, Str8List *list) {
  Str8Array array = {
      .count = list->node_count,
      .size = list->node_count,
      .v = push_array_no_zero(arena, Str8, list->node_count),
  };
  u64 idx = 0;
  for (Str8Node *n = list->first; n != 0; n = n->next, idx += 1) {
    array.v[idx] = n->str;
  }
  return array;
}

Str8Array str8_array_copy_from_list(Arena *arena, Str8List *list) {
  Str8Array array = {
      .count = list->node_count,
      .size = list->node_count,
      .v = push_array_no_zero(arena, Str8, list->node_count),
  };
  u64 idx = 0;
  for (Str8Node *n = list->first; n != 0; n = n->next, idx += 1) {
    array.v[idx] = str8_copy(arena, n->str);
  }
  return array;
}

Str8Array str8_array_reserve(Arena *arena, u64 count) {
  Str8Array arr = {
      .count = 0,
      .size = count,
      .v = push_array(arena, Str8, count),
  };
  return arr;
}

Str8Array str8_array_copy(Arena *arena, Str8Array array) {
  Str8Array result = {
      .count = array.count,
      .size = array.count,
      .v = push_array_no_zero(arena, Str8, array.count),
  };
  for (u64 idx = 0; idx < result.count; idx++) {
    result.v[idx] = str8_copy(arena, array.v[idx]);
  }
  return result;
}
