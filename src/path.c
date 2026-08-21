#include "bace/strings.h"
#include "bace/thread_context.h"
#include "bace/path.h"

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
    if (char_is_slash(str.str[p])) {
      break;
    }
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
    if (char_is_slash(str.str[p])) {
      break;
    }
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
  Str8List new_path = {0};

  for (Str8Node *node = path->first, *next = 0; node != 0; node = next) {
    // save next now before we mutate the node
    next = node->next;

    // skip empty segments
    if (node->str.size == 0) {
      continue;
    }

    // skip single dots
    if (node->str.size == 1 && node->str.str[0] == '.') {
      continue;
    }

    // handle double dots (backtrack)
    if (node->str.size == 2 && node->str.str[0] == '.' && node->str.str[1] == '.') {
      bool can_pop = false;
      if (new_path.node_count > 0) {
        Str8Node *last = new_path.last;
        bool is_dotdot =
            (last->str.size == 2 && last->str.str[0] == '.' && last->str.str[1] == '.');
        bool is_windows_root =
            (style == PathStyle_WindowsAbsolute && new_path.node_count == 1);

        // we can only pop if the previous node isn't another ".."
        // and isn't the root drive letter of an absolute Windows path
        if (!is_dotdot && !is_windows_root) {
          can_pop = true;
        }
      }

      if (can_pop) {
        // pop the last directory
        new_path.node_count -= 1;
        new_path.total_size -= new_path.last->str.size;

        if (new_path.node_count == 0) {
          new_path.first = 0;
          new_path.last = 0;
        } else {
          // find the new tail
          Str8Node *tail = new_path.first;
          for (u64 i = 1; i < new_path.node_count; ++i) {
            tail = tail->next;
          }
          new_path.last = tail;
          new_path.last->next = 0;
        }
        continue;
      } else {
        // if this is an absolute path at the root, ignore the backtrack.
        if (style == PathStyle_WindowsAbsolute && new_path.node_count == 1) {
          continue;
        }
        if (style == PathStyle_UnixAbsolute && new_path.node_count == 0) {
          continue;
        }

        node->next = 0;
        str8_list_push_node(&new_path, node);
        continue;
      }
    }

    // normal folder/file
    node->next = 0; // sever old forward links before pushing
    str8_list_push_node(&new_path, node);
  }

  // replace original list with our safely constructed one
  *path = new_path;
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

  // gather path parts
  Str8 dst_name = str8_skip_last_slash(dst);
  Str8 src_folder = str8_chop_last_slash(src);
  Str8 dst_folder = str8_chop_last_slash(dst);

  Str8List src_folders = str8_split_path(scratch.arena, src_folder);
  Str8List dst_folders = str8_split_path(scratch.arena, dst_folder);

  // count # of backtracks to get from src -> dest
  u64 num_backtracks = src_folders.node_count;
  Str8Node *src_node = src_folders.first;
  Str8Node *dst_node = dst_folders.first;
  while (src_node != 0 && dst_node != 0) {
    if (str8_match(src_node->str, dst_node->str,
                   path_match_flags_from_os(OperatingSystem_CURRENT))) {
      num_backtracks -= 1;
      src_node = src_node->next;
      dst_node = dst_node->next;
    } else {
      break;
    }
  }

  // only build relative string if # of backtracks is not the entire `src`.
  // if getting to `dst` from `src` requires erasing the entire `src`, then the
  // only possible way to get to `dst` from `src` is via absolute path.
  Str8 dst_path = {0};
  if (num_backtracks >= src_folders.node_count) {
    dst_path = dst;
  } else {
    // build backtrack parts
    Str8List dst_path_strs = {0};
    for (u64 idx = 0; idx < num_backtracks; idx += 1) {
      str8_list_push(scratch.arena, &dst_path_strs, str8_lit(".."));
    }

    // build parts of dst which are unique from src
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

    // build file name
    str8_list_push(scratch.arena, &dst_path_strs, dst_name);

    // join
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
    Str8List src_parts = str8_split_path(scratch.arena, src);
    Str8List dst_parts = str8_split_path(scratch.arena, dst);

    if (dst_parts.node_count > 0) {
      if (src_parts.node_count == 0) {
        src_parts = dst_parts;
      } else {
        src_parts.last->next = dst_parts.first;
        src_parts.last = dst_parts.last;
        src_parts.node_count += dst_parts.node_count;
        src_parts.total_size += dst_parts.total_size;
      }
    }

    PathStyle src_style = path_style_from_str8(src);
    str8_path_list_resolve_dots_in_place(&src_parts, src_style);
    result = str8_path_list_join_by_style(arena, &src_parts, src_style);

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
