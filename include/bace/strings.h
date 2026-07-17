#ifndef _H_BACE_STRINGS
#define _H_BACE_STRINGS

#include "arena.h"
#include "base.h"

#include <stdarg.h>

typedef struct Str8 {
  u8 *str;
  u64 size;
} Str8;

// character classification
bool char_is_space(u8 c);
bool char_is_upper(u8 c);
bool char_is_lower(u8 c);
bool char_is_alpha(u8 c);
bool char_is_digit(u8 c, u32 base);
bool char_is_slash(u8 c);

// char conversion
u8 char_to_lower(u8 c);
u8 char_to_upper(u8 c);

u64 cstring8_length(const u8 *c);

// constructors
Str8 str8(u8 *str, u64 size);
Str8 str8_zero(void);
Str8 str8_cstring(const char *c);
Str8 str8_range(u8 *first, u8 *one_past_last);

Str8 str8_to_upper(Arena *arena, Str8 string);
Str8 str8_to_lower(Arena *arena, Str8 string);

#define s(str) str8_lit(str)
#define str8_lit(str) str8((u8 *)(str), sizeof(str) - 1)
#define str8_array(s, c) str8((u8 *)(s), sizeof(*(s)) * (c))
#define str8_lit_comp(str) {(u8 *)(str), sizeof(str) - 1}

// copytin and formatting
Str8 str8_cat(Arena *arena, Str8 s1, Str8 s2);
Str8 str8_copy(Arena *arena, Str8 s);
Str8 str8fv(Arena *arena, const char *fmt, va_list args);
Str8 str8f(Arena *arena, char *fmt, ...);

// slicing
Str8 str8_substr(Str8 str, u64 min, u64 max);
Str8 str8_prefix(Str8 str, u64 size);
Str8 str8_skip(Str8 str, u64 amt);
Str8 str8_postfix(Str8 str, u64 size);
Str8 str8_chop(Str8 str, u64 amt);
Str8 str8_chop_line(Str8 *str);
Str8 str8_skip_chop(Str8 string, bool (*predicate)(u8 c));
Str8 str8_skip_chop_whitespace(Str8 string);
Str8 str8_skip_chop_slashes(Str8 string);

// matching
typedef u64 StringMatchFlags;
enum {
  StringMatchFlag_CaseInsensitive = (1 << 0),
  StringMatchFlag_SlashInsensitive = (1 << 1),
  StringMatchFlag_RightSideSloppy = (1 << 2),
};

bool str8_match(Str8 a, Str8 b, StringMatchFlags flags);
bool str8_match_wildcard(Str8 string, Str8 pattern, StringMatchFlags flags);
u64 str8_find_needle(Str8 string, u64 start_pos, Str8 needle, StringMatchFlags flags);
u64 str8_find_needle_reverse(Str8 string, u64 start_pos, Str8 needle,
                             StringMatchFlags flags);
bool str8_is_before(Str8 a, Str8 b);

#define str8_matchi(a, b) str8_match(a, b, StringMatchFlag_CaseInsensitive)
#define str8_match_lit(a_lit, b, flags) str8_match(str8_lit(a_lit), (b), (flags))
#define str8_match_cstr(a_cstr, b, flags) str8_match(str8_cstring(a_cstr), (b), (flags))

#define str8_ends_with(string, end, flags)                                               \
  str8_match(str8_postfix((string), (end).size), (end), (flags))
#define str8_starts_with(string, prefix, flags)                                          \
  str8_match(str8_prefix(string, expected_prefix.size), expected_prefix, flags)

// string list & array types
typedef struct Str8Node {
  struct Str8Node *next;
  Str8 str;
} Str8Node;

typedef struct Str8List {
  Str8Node *first;
  Str8Node *last;
  u64 node_count;
  u64 total_size;
} Str8List;

typedef struct Str8Array {
  Str8 *v;
  u64 size;
  u64 count;
} Str8Array;

Str8Node *str8_list_push_node(Str8List *list, Str8Node *node);
Str8Node *str8_list_push_node_set_string(Str8List *list, Str8Node *node, Str8 str);
Str8Node *str8_list_push_node_front(Str8List *list, Str8Node *node);
Str8Node *str8_list_push_node_front_set_string(Str8List *list, Str8Node *node, Str8 str);

Str8Node *str8_list_push(Arena *arena, Str8List *list, Str8 str);
Str8Node *str8_list_push_front(Arena *arena, Str8List *list, Str8 str);
void str8_list_concat_in_place(Str8List *list, Str8List *to_push);
Str8Node *str8_list_pushf(Arena *arena, Str8List *list, char *fmt, ...);
Str8Node *str8_list_push_frontf(Arena *arena, Str8List *list, char *fmt, ...);

Str8Node *str8_list_pop_front(Str8List *list);
Str8List str8_list_copy(Arena *arena, Str8List *list);
Str8List str8_list_substr(Arena *arena, Str8List list, u64 min, u64 max);

#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

// split & join
typedef u64 StringSplitFlags;
enum {
  StringSplitFlag_KeepEmpties = (1 << 0),
};

typedef struct StringJoin {
  Str8 pre;
  Str8 sep;
  Str8 post;
} StringJoin;

Str8List str8_split(Arena *arena, Str8 string, u8 *split_chars, u64 split_char_count,
                    StringSplitFlags flags);
Str8List str8_split_by_string_chars(Arena *arena, Str8 string, Str8 split_chars,
                                    StringSplitFlags flags);
Str8 str8_list_join(Arena *arena, Str8List *list, StringJoin *optional_params);

#endif // !_H_BACE_STRINGS
