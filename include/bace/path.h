// path.h
//
// provides functions for handling paths
//
// references: raddebugger

#ifndef _H_PATH
#define _H_PATH

#include "bace/strings.h"

typedef enum PathStyle {
  PathStyle_Null,
  PathStyle_Relative,
  PathStyle_WindowsAbsolute,
  PathStyle_UnixAbsolute,

#ifdef OS_WINDOWS
  PathStyle_SystemAbsolute = PathStyle_WindowsAbsolute,
#elif OS_LINUX
  PathStyle_SystemAbsolute = PathStyle_UnixAbsolute,
#else
#error absolute path style undefined for this os.
#endif
} PathStyle;

// path helpers

Str8 str8_chop_last_slash(Str8 path);
Str8 str8_skip_last_slash(Str8 path);
Str8 str8_chop_last_dot(Str8 path);
Str8 str8_skip_last_dot(Str8 path);

PathStyle path_style_from_str8(Str8 string);
Str8List str8_split_path(Arena *arena, Str8 string);
void str8_path_list_resolve_dots_in_place(Str8List *path, PathStyle style);
Str8 str8_path_list_join_by_style(Arena *arena, Str8List *path, PathStyle style);

Str8 path_relative_dst_from_absolute_dst_src(Arena *arena, Str8 dst, Str8 src);
Str8 path_absolute_dst_from_relative_dst_src(Arena *arena, Str8 dst, Str8 src);

Str8List path_normalized_list_from_string(Arena *arena, Str8 path, PathStyle *style_out);
Str8 path_normalized_from_string(Arena *arena, Str8 path);
bool path_match_normalized(Str8 left, Str8 right);

PathStyle path_style_from_string(Str8 string);
Str8 string_from_path_style(PathStyle style);
Str8 path_separator_string_from_style(PathStyle style);
StringMatchFlags path_match_flags_from_os(OperatingSystem os);
Str8 path_convert_slashes(Arena *arena, Str8 path, PathStyle path_style);
Str8 path_replace_file_extension(Arena *arena, Str8 file_name, Str8 ext);

#endif // !_H_PATH
