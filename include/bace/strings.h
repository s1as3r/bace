// strings.h
//
// provides a length-carrying, UTF-8/byte-string type and the
// construction, formatting, slicing, matching, list, and split/join
// utilities built on top of it.
//
// ownership model: unless documented otherwise, functions that need to
// allocate memory (copies, formatting, concatenation, list nodes, etc.)
// take an explicit `Arena *` and carve the result out of it.
// functions that only need to *view* existing memory (e.g. `str8_substr`, `str8_prefix`)
// return a `Str8` that aliases the input's backing storage and do not allocate.
// the caller must ensure that storage outlives the returned `Str8`.

#ifndef _H_STRINGS
#define _H_STRINGS

#include "arena.h"
#include "base.h"

#include <stdarg.h>

// a byte string: `str` points to `size` bytes, *NOT* guaranteed to be
// null-terminated.
typedef struct Str8 {
  u8 *str;
  u64 size;
} Str8;

// character classification

bool char_is_space(u8 c);
bool char_is_upper(u8 c);
bool char_is_lower(u8 c);
bool char_is_alpha(u8 c);

// returns `true` if `c` is a valid digit in the given `base` .
// `base` must be in (0, 16].
// `c` must be in the ASCII range (0-127)
bool char_is_digit(u8 c, u32 base);
// returns `true` if `c` is '/' or '\\' (used for OS-path-separator-agnostic logic).
bool char_is_slash(u8 c);

// char conversion
//
// ASCII-only case conversion. non-alphabetic bytes are returned unchanged.

u8 char_to_lower(u8 c);
u8 char_to_upper(u8 c);

// length of a null-terminated byte string `str`.
// `str` must be null-terminated.
u64 cstring8_length(const u8 *str);

// constructors
//
// none of these allocate or copy.
// all returned `Str8` values alias the memory passed in, which must remain
// valid for as long as the `Str8` is used.
// use `str8_copy` if an independent copy is needed.

// wraps an existing `size`-byte buffer as a `Str8`
Str8 str8(u8 *str, u64 size);

// returns the empty string ({0, 0}).
Str8 str8_zero(void);

// wraps a null-terminated C string, computing its length via `cstring8_length`.
Str8 str8_cstring(const char *c);

// wraps the half-open byte range [`first`, `one_past_last`) as a `Str8`.
Str8 str8_range(u8 *first, u8 *one_past_last);

// returns a newly-allocated upper-cased copy of `string`.
// ASCII-only case conversion.
Str8 str8_to_upper(Arena *arena, Str8 string);

// returns a newly-allocated lower-cased copy of `string`.
// ASCII-only case conversion.
Str8 str8_to_lower(Arena *arena, Str8 string);

// string literal helpers.
// `s`/`str8_lit` build a `Str8` view over a C string.
// only valid for actual string-literal arguments (relies on  sizeof``).
// `str8_array`
// builds a `Str8` over `c` elements of an arbitrary array `s`.
// `str8_lit_comp` produces a `Str8` compound literal
#define s(str) str8_lit(str)
#define str8_lit(str) str8((u8 *)(str), sizeof(str) - 1)
#define str8_array(s, c) str8((u8 *)(s), sizeof(*(s)) * (c))
#define str8_lit_comp(str) {(u8 *)(str), sizeof(str) - 1}

// copying and formatting

// returns a new string containing the concatenation of `s1` and `s2`.
Str8 str8_cat(Arena *arena, Str8 s1, Str8 s2);

// returns a copy of `s`
// the returned string is null terminated.
Str8 str8_copy(Arena *arena, Str8 s);

// `printf`-style formatting into an arena-allocated `Str8`
// `fmt` uses standard printf format-specifier syntax.
Str8 str8fv(Arena *arena, const char *fmt, va_list args);
// `printf`-style formatting into an arena-allocated `Str8`.
Str8 str8f(Arena *arena, char *fmt, ...);

// slicing
// returns the sub-range [min, max) of `str`
// (byte offsets, not clamped to character boundaries for multi-byte UTF-8).
Str8 str8_substr(Str8 str, u64 min, u64 max);

// first `size` bytes of `str`.
Str8 str8_prefix(Str8 str, u64 size);

// `str` with its first `amt` bytes removed.
Str8 str8_skip(Str8 str, u64 amt);

// last `size` bytes of `str`.
Str8 str8_postfix(Str8 str, u64 size);

// `str` with its last `amt` bytes removed.
Str8 str8_chop(Str8 str, u64 amt);

// splits off and returns the first line from `*str`, advancing `*str` past
// the consumed line.
Str8 str8_chop_line(Str8 *str);

// removes bytes matched by `predicate` from the ends of `string` and
// returns the remainder.
Str8 str8_skip_chop(Str8 string, bool (*predicate)(u8 c));

// trims leading/trailing whitespace from `string`.
Str8 str8_skip_chop_whitespace(Str8 string);

// trims leading/trailing slash characters from `string`.
Str8 str8_skip_chop_slashes(Str8 string);

// matching

typedef u64 StringMatchFlags;
enum {
  StringMatchFlag_CaseInsensitive = (1 << 0),
  // treat '/' and '\\' as equivalent when comparing.
  StringMatchFlag_SlashInsensitive = (1 << 1),
  // a prefix match not requiring equal length.
  StringMatchFlag_RightSideSloppy = (1 << 2),
};

// byte-for-byte equality of `a` and `b` under `flags`.
bool str8_match(Str8 a, Str8 b, StringMatchFlags flags);

// glob-style wildcard match of `string` against `pattern`.
bool str8_match_wildcard(Str8 string, Str8 pattern, StringMatchFlags flags);

// finds the first occurrence of `needle` in `string` at or after
// `start_pos`, returning its byte offset, or `string.size` if not found.
u64 str8_find_needle(Str8 string, u64 start_pos, Str8 needle, StringMatchFlags flags);

// searches for the last (rightmost) occurrence of `needle` within the
// region of `string` obtained by trimming `start_pos` bytes off the *end*
// returns the offset one-past-the-end of the match if found, or 0 if not found.
u64 str8_find_needle_reverse(Str8 string, u64 start_pos, Str8 needle,
                             StringMatchFlags flags);

// lexicographic ordering predicate (byte-wise), suitable for use as a sort
// comparator.
bool str8_is_before(Str8 a, Str8 b);

#define str8_matchi(a, b) str8_match(a, b, StringMatchFlag_CaseInsensitive)
#define str8_match_lit(a_lit, b, flags) str8_match(str8_lit(a_lit), (b), (flags))
#define str8_match_cstr(a_cstr, b, flags) str8_match(str8_cstring(a_cstr), (b), (flags))

#define str8_ends_with(string, end, flags)                                               \
  str8_match(str8_postfix((string), (end).size), (end), (flags))
#define str8_starts_with(string, prefix, flags)                                          \
  str8_match(str8_prefix((string), (prefix).size), (prefix), (flags))

// string list & array types

typedef struct Str8Node {
  struct Str8Node *next;
  Str8 str;
} Str8Node;

typedef struct Str8List {
  Str8Node *first;
  Str8Node *last;
  // number of nodes currently in the list.
  u64 node_count;
  // sum of `str.size` across all nodes.
  u64 total_size;
} Str8List;

// flat array of `count` strings backed by storage `v` of `size` elements
typedef struct Str8Array {
  Str8 *v;
  u64 size;
  u64 count;
} Str8Array;

// node-based list operations. caller allocates/owns `node` and these functions
// just link it in.
// prefer the allocating `str8_list_push*` variants below unless you specifically
// need to control node allocation yourself.
Str8Node *str8_list_push_node(Str8List *list, Str8Node *node);
Str8Node *str8_list_push_node_set_string(Str8List *list, Str8Node *node, Str8 str);
Str8Node *str8_list_push_node_front(Str8List *list, Str8Node *node);
Str8Node *str8_list_push_node_front_set_string(Str8List *list, Str8Node *node, Str8 str);

// allocates a new node out of `arena`, sets its string to `str`, and
// appends it to `list`. returns the new node.
Str8Node *str8_list_push(Arena *arena, Str8List *list, Str8 str);

// allocates a new node out of `arena`, sets its string to `str`, and
// prepends it to `list`. returns the new node.
Str8Node *str8_list_push_front(Arena *arena, Str8List *list, Str8 str);

// appends all nodes of `to_push` onto the end of `list` in place.
void str8_list_concat_in_place(Str8List *list, Str8List *to_push);

// `printf`-style formatting directly into a new node appended to `list`,
// allocated from `arena`.
Str8Node *str8_list_pushf(Arena *arena, Str8List *list, char *fmt, ...);

// `printf`-style formatting directly into a new node prepended to `list`,
// allocated from `arena`.
Str8Node *str8_list_push_frontf(Arena *arena, Str8List *list, char *fmt, ...);

// removes and returns the first node of `list`.
// returns `null` if `list` is empty.
Str8Node *str8_list_pop_front(Str8List *list);

// deep-copy `list`  into a new list allocated from `arena`, leaving `list` untouched.
Str8List str8_list_copy(Arena *arena, Str8List *list);

// returns the sub-list of nodes corresponding to the logical
// character range [`min`, `max`) across the whole concatenated list,
// allocating any newly-needed nodes/strings from `arena`.
Str8List str8_list_substr(Arena *arena, Str8List list, u64 min, u64 max);

#define str8_list_first(list) ((list)->first ? (list)->first->string : str8_zero())

// split & join

typedef u64 StringSplitFlags;
enum {
  StringSplitFlag_KeepEmpties = (1 << 0),
};

// parameters for `str8_list_join`: `pre`/`post` are prepended/
// appended once to the whole joined result, and `sep` is inserted between
// each pair of adjacent list entries.
// pass `null` to str8_list_join to join with no separator/pre/post.
typedef struct StringJoin {
  Str8 pre;
  Str8 sep;
  Str8 post;
} StringJoin;

// splits `string` on any byte in `split_chars`, returning the pieces as a new
// `Str8List` allocated from `arena`.
Str8List str8_split(Arena *arena, Str8 string, u8 *split_chars, u64 split_char_count,
                    StringSplitFlags flags);

// same as `str8_split`, but takes the delimiter set as a `Str8`.
Str8List str8_split_by_string_chars(Arena *arena, Str8 string, Str8 split_chars,
                                    StringSplitFlags flags);

// joins `list`'s entries into a single `Str8`, allocated from `arena`,
// optionally using `optional_params` for separator/pre/post text.
Str8 str8_list_join(Arena *arena, Str8List *list, StringJoin *optional_params);

#endif // !_H_STRINGS
