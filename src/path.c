#include "bace/thread_context.h"
#include "bace/path.h"
#include <string.h>

global struct {
  Str8 string;
  PathStyle path_style;
} g_path_style_map[] = {
    {str8_lit_comp(""), PathStyle_Null},
    {str8_lit_comp("relative"), PathStyle_Relative},
    {str8_lit_comp("windows"), PathStyle_WindowsAbsolute},
    {str8_lit_comp("unix"), PathStyle_UnixAbsolute},
    {str8_lit_comp("system"), PathStyle_SystemAbsolute},
};

Str8 str8_chop_last_slash(Str8 str) {
  if (str.size > 0) {
    u8 *ptr = str.str + str.size - 1;
    for (; ptr >= str.str; ptr -= 1) {
      if (char_is_slash(*ptr)) {
        break;
      }
    }
    if (ptr >= str.str) {
      str.size = (u64)(ptr - str.str);
    } else {
      str.size = 0;
    }
  }
  return str;
}

Str8 str8_skip_last_slash(Str8 str) {
  if (str.size > 0) {
    u8 *ptr = str.str + str.size - 1;
    for (; ptr >= str.str; ptr -= 1) {
      if (char_is_slash(*ptr)) {
        break;
      }
    }
    if (ptr >= str.str) {
      ptr += 1;
      str.size = (u64)(str.str + str.size - ptr);
      str.str = ptr;
    }
  }
  return str;
}

Str8 str8_chop_last_dot(Str8 str) {
  Str8 result = str;
  u64 p = str.size;
  for (; p > 0;) {
    p -= 1;
    if (str.str[p] == '.') {
      result = str8_prefix(str, p);
      break;
    }
  }
  return result;
}

Str8 str8_skip_last_dot(Str8 str) {
  Str8 result = str;
  u64 p = str.size;
  for (; p > 0;) {
    p -= 1;
    if (str.str[p] == '.') {
      result = str8_skip(str, p + 1);
      break;
    }
  }
  return result;
}

PathStyle path_style_from_str8(Str8 string) {
  PathStyle result = PathStyle_Relative;
  if (string.size >= 1 && string.str[0] == '/') {
    result = PathStyle_UnixAbsolute;
  } else if (string.size >= 2 && char_is_alpha(string.str[0]) && string.str[1] == ':') {
    if (string.size == 2 || char_is_slash(string.str[2])) {
      result = PathStyle_WindowsAbsolute;
    }
  }
  return result;
}

Str8List str8_split_path(Arena *arena, Str8 string) {
  Str8List result = str8_split(arena, string, (u8 *)"/\\", 2, 0);
  return result;
}

void str8_path_list_resolve_dots_in_place(Str8List *path, PathStyle style) {
  Temp scratch = scratch_begin(0, 0);
  typedef struct Str8MetaNode Str8MetaNode;
  struct Str8MetaNode {
    Str8MetaNode *next;
    Str8Node *node;
  };
  Str8MetaNode *stack = 0;
  Str8MetaNode *free_meta_node = 0;
  Str8Node *first = path->first;
  memset(path, 0, sizeof(*path));
  for (Str8Node *node = first, *next = 0; node != 0; node = next) {
    // save next now
    next = node->next;

    // cases:
    if (node == first && style == PathStyle_WindowsAbsolute) {
      goto save_without_stack;
    }
    if (node->str.size == 1 && node->str.str[0] == '.') {
      goto do_nothing;
    }
    if (node->str.size == 2 && node->str.str[0] == '.' && node->str.str[1] == '.') {
      if (stack != 0) {
        goto eliminate_stack_top;
      } else {
        goto save_without_stack;
      }
    }
    goto save_with_stack;

  // handlers:
  save_with_stack: {
    str8_list_push_node(path, node);
    Str8MetaNode *stack_node = free_meta_node;
    if (stack_node != 0) {
      sll_stack_pop(free_meta_node);
    } else {
      stack_node = push_array_no_zero(scratch.arena, Str8MetaNode, 1);
    }
    sll_stack_push(stack, stack_node);
    stack_node->node = node;
    continue;
  }

  save_without_stack: {
    str8_list_push_node(path, node);
    continue;
  }

  eliminate_stack_top: {
    path->node_count -= 1;
    path->total_size -= stack->node->str.size;
    sll_stack_pop(stack);
    if (stack == 0) {
      path->last = path->first;
    } else {
      path->last = stack->node;
    }
    continue;
  }

  do_nothing:
    continue;
  }
  scratch_end(scratch);
}

Str8 str8_path_list_join_by_style(Arena *arena, Str8List *path, PathStyle style) {
  StringJoin params = {0};
  switch (style) {
  case PathStyle_Null: {
  } break;
  case PathStyle_Relative: // fallthrough
  case PathStyle_WindowsAbsolute: {
    params.sep = str8_lit("/");
  } break;

  case PathStyle_UnixAbsolute: {
    params.pre = str8_lit("/");
    params.sep = str8_lit("/");
  } break;
  }
  Str8 result = str8_list_join(arena, path, &params);
  return result;
}
Str8 path_relative_dst_from_absolute_dst_src(Arena *arena, Str8 dst, Str8 src) {
  Temp scratch = scratch_begin(&arena, 1);

  // rjf: gather path parts
  Str8 dst_name = str8_skip_last_slash(dst);
  Str8 src_folder = src;
  Str8 dst_folder = str8_chop_last_slash(dst);
  Str8List src_folders = str8_split_path(scratch.arena, src_folder);
  Str8List dst_folders = str8_split_path(scratch.arena, dst_folder);

  // rjf: count # of backtracks to get from src -> dest
  u64 num_backtracks = src_folders.node_count;
  for (Str8Node *src_n = src_folders.first, *bp_n = dst_folders.first;
       src_n != 0 && bp_n != 0; src_n = src_n->next, bp_n = bp_n->next) {
    if (str8_match(src_n->str, bp_n->str,
                   path_match_flags_from_os(OperatingSystem_CURRENT))) {
      num_backtracks -= 1;
    } else {
      break;
    }
  }

  // rjf: only build relative string if # of backtracks is not the entire `src`.
  // if getting to `dst` from `src` requires erasing the entire `src`, then the
  // only possible way to get to `dst` from `src` is via absolute path.
  Str8 dst_path = {0};
  if (num_backtracks >= src_folders.node_count) {
    dst_path = dst;
  } else {
    // rjf: build backtrack parts
    Str8List dst_path_strs = {0};
    for (u64 idx = 0; idx < num_backtracks; idx += 1) {
      str8_list_push(scratch.arena, &dst_path_strs, str8_lit(".."));
    }

    // rjf: build parts of dst which are unique from src
    {
      bool unique_from_src = 0;
      for (Str8Node *src_n = src_folders.first, *bp_n = dst_folders.first; bp_n != 0;
           bp_n = bp_n->next) {
        if (!unique_from_src &&
            (src_n == 0 ||
             !str8_match(src_n->str, bp_n->str,
                         path_match_flags_from_os(OperatingSystem_CURRENT)))) {
          unique_from_src = 1;
        }
        if (unique_from_src) {
          str8_list_push(scratch.arena, &dst_path_strs, bp_n->str);
        }
        if (src_n != 0) {
          src_n = src_n->next;
        }
      }
    }

    // rjf: build file name
    str8_list_push(scratch.arena, &dst_path_strs, dst_name);

    // rjf: join
    StringJoin join = {0};
    {
      join.sep = str8_lit("/");
    }
    dst_path = str8_list_join(arena, &dst_path_strs, &join);
  }
  scratch_end(scratch);
  return dst_path;
}
Str8 path_absolute_dst_from_relative_dst_src(Arena *arena, Str8 dst, Str8 src) {
  Str8 result = dst;
  PathStyle dst_style = path_style_from_str8(dst);
  if (dst.size != 0 && dst_style == PathStyle_Relative) {
    Temp scratch = scratch_begin(&arena, 1);
    Str8 dst_from_src_absolute = str8f(scratch.arena, "%S/%S", src, dst);
    Str8List dst_from_src_absolute_parts =
        str8_split_path(scratch.arena, dst_from_src_absolute);
    PathStyle dst_from_src_absolute_style = path_style_from_str8(src);
    str8_path_list_resolve_dots_in_place(&dst_from_src_absolute_parts,
                                         dst_from_src_absolute_style);
    result = str8_path_list_join_by_style(arena, &dst_from_src_absolute_parts,
                                          dst_from_src_absolute_style);
    scratch_end(scratch);
  }
  return result;
}

Str8List path_normalized_list_from_string(Arena *arena, Str8 path_string,
                                          PathStyle *style_out) {
  PathStyle path_style = path_style_from_str8(path_string);
  Str8List path = str8_split_path(arena, path_string);

  str8_path_list_resolve_dots_in_place(&path, path_style);

  if (style_out != 0) {
    *style_out = path_style;
  }
  return path;
}

Str8 path_normalized_from_string(Arena *arena, Str8 path_string) {
  Temp scratch = scratch_begin(&arena, 1);
  PathStyle style = PathStyle_Relative;
  Str8List path = path_normalized_list_from_string(scratch.arena, path_string, &style);
  Str8 result = str8_path_list_join_by_style(arena, &path, style);
  scratch_end(scratch);
  return result;
}

bool path_match_normalized(Str8 left, Str8 right) {
  Temp scratch = scratch_begin(0, 0);
  Str8 left_normalized = path_normalized_from_string(scratch.arena, left);
  Str8 right_normalized = path_normalized_from_string(scratch.arena, right);
  bool result =
      str8_match(left_normalized, right_normalized, StringMatchFlag_CaseInsensitive);
  scratch_end(scratch);
  return result;
}

PathStyle path_style_from_string(Str8 string) {
  for (u64 i = 0; i < array_count(g_path_style_map); ++i) {
    if (str8_match(g_path_style_map[i].string, string, StringMatchFlag_CaseInsensitive)) {
      return g_path_style_map[i].path_style;
    }
  }
  return PathStyle_Null;
}

Str8 string_from_path_style(PathStyle style) {
  assert((u64)style < array_count(g_path_style_map));
  return g_path_style_map[style].string;
}

Str8 path_separator_string_from_style(PathStyle style) {
  Str8 result = str8_zero();
  switch (style) {
  case PathStyle_Null:
    break;
  case PathStyle_Relative:
    break;
  case PathStyle_WindowsAbsolute:
    result = str8_lit("\\");
    break;
  case PathStyle_UnixAbsolute:
    result = str8_lit("/");
    break;
  }
  return result;
}

StringMatchFlags path_match_flags_from_os(OperatingSystem os) {
  StringMatchFlags flags = StringMatchFlag_SlashInsensitive;
  switch (os) {
  default: {
  } break;
  case OperatingSystem_Windows: {
    flags |= StringMatchFlag_CaseInsensitive;
  } break;
  case OperatingSystem_Linux:
  case OperatingSystem_Mac: {
  } break;
  }
  return flags;
}

Str8 path_convert_slashes(Arena *arena, Str8 path, PathStyle path_style) {
  Temp scratch = scratch_begin(&arena, 1);
  Str8List list = str8_split_path(scratch.arena, path);
  StringJoin join = {0};
  join.sep = path_separator_string_from_style(path_style);
  Str8 result = str8_list_join(arena, &list, &join);
  scratch_end(scratch);
  return result;
}

Str8 path_replace_file_extension(Arena *arena, Str8 file_name, Str8 ext) {
  Str8 file_name_no_ext = str8_chop_last_dot(file_name);
  Str8 result = str8f(arena, "%S.%S", file_name_no_ext, ext);
  return result;
}
