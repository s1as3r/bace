// path.h
//
// provides functions for handling paths built on top of `Str8`.
//
// references: raddebugger, mr4th

#ifndef _H_PATH
#define _H_PATH

#include "bace/strings.h"

typedef enum PathStyle {
  // not a recognized/valid path style
  PathStyle_Null,
  // a relative path
  PathStyle_Relative,
  // an absolute windows-style path (e.g "C:\dev\bace")
  PathStyle_WindowsAbsolute,
  // an absolute unix-style path (e.g "/home/foo")
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
// none of these allocate, all return views aliasing `path`'s backing memory.

// returns everything before the last path separator, excluding the separator itself.
// returns an empty string if `path` contains no separator.
Str8 str8_chop_last_slash(Str8 path);

// returns everything after the last path separator, excluding the separator itslef.
// returns `path` unchanged if it contains no separator.
Str8 str8_skip_last_slash(Str8 path);

// returns `path` with its last '.'-delimited extension removed.
// retuns `path` unchanged if it contains no '.'.
Str8 str8_chop_last_dot(Str8 path);

// returns everything after the last '.'.
// returns `path` unchanged if it contains no '.'.
Str8 str8_skip_last_dot(Str8 path);

// path classification, splitting and joining

// classifies `string`'s absolute-path style by inspecting its prefix.
PathStyle path_style_from_str8(Str8 string);

// splits `string` into path components on '/' or '\\', discarding empty components.
Str8List str8_split_path(Arena *arena, Str8 string);

// resolves "." and ".." components of `path` in place.
void str8_path_list_resolve_dots_in_place(Str8List *path, PathStyle style);

// joins `path`'s components bnack into a single string
Str8 str8_path_list_join_by_style(Arena *arena, Str8List *path, PathStyle style);

// relative/absolute path conversions

// returns the relative path used to navigate from `src` to `dst`
Str8 path_relative_dst_from_absolute_dst_src(Arena *arena, Str8 dst, Str8 src);

// returns the absolute path for `dst`
Str8 path_absolute_dst_from_relative_dst_src(Arena *arena, Str8 dst, Str8 src);

// normalization

// splits `path` into components and resolves dots
// if `style_out` is not-null, it receives the `PathStyle` detected from `path` itself
Str8List path_normalized_list_from_string(Arena *arena, Str8 path, PathStyle *style_out);

// wrapper around `path_normalized_list_from_string` + `str8_path_list_join_by_style`
Str8 path_normalized_from_string(Arena *arena, Str8 path);

// normalizes both `left` nad `right` and compares the result with
// `StringMatchFlag_CaseInsensitive`
bool path_match_normalized(Str8 left, Str8 right);

// style lookup

// looks up a `PathStyle` by case-insensitive name ("relative", "windows", "unix" or
// "system")
PathStyle path_style_from_string(Str8 string);

// returns the human-readable name for `style`
Str8 string_from_path_style(PathStyle style);

// returns the conventional path separator character for `style`
Str8 path_separator_string_from_style(PathStyle style);

// returns the `StringMatchFlags` appropriate for comparing path components on the given
// OS
StringMatchFlags path_match_flags_from_os(OperatingSystem os);

// reformatting

// returns `path` but with every separator in replaced by the separator string that
// `path_separator_string_from_style` returns
Str8 path_convert_slashes(Arena *arena, Str8 path, PathStyle path_style);

// returns `file_name` with its extension replaced by `ext`.
// appends `ext` to the end of `file_name` if it is empty.
Str8 path_replace_file_extension(Arena *arena, Str8 file_name, Str8 ext);

#endif // !_H_PATH
