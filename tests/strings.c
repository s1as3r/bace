#include "bace/strings.h"
#include "bace/bace.h"
#include "bace/thread_context.h"

#define TEST_INIT bace_os_state_init()
#include "acutest.h"

void test_str8_list_substr(void) {
  Str8List zero_list = {0};

  Temp scratch = scratch_begin(0, 0);
  Arena *arena = scratch.arena;
  {
    TEST_CASE("join & list substr");
    Str8List list = {0};
    str8_list_pushf(arena, &list, "a");
    str8_list_pushf(arena, &list, "b");
    str8_list_pushf(arena, &list, "c");

    {
      Str8 res = str8_list_join(arena, &list, 0);
      TEST_CHECK_(str8_match(res, s("abc"), 0), "%s matches abc", res.str);
    }

    {
      Str8List sub = str8_list_substr(arena, list, 0, 2);
      Str8 res = str8_list_join(arena, &sub, 0);
      TEST_CHECK_(str8_match(res, s("ab"), 0), "%s matches ab", res.str);
    }

    {
      Str8List sub = str8_list_substr(arena, list, 0, UINT64_MAX);
      Str8 res = str8_list_join(arena, &sub, 0);
      TEST_CHECK_(str8_match(res, s("abc"), 0), "%s matches abc", res.str);
    }
  }

  {
    TEST_CASE("list substr 2");
    Str8List list = {0};
    str8_list_pushf(arena, &list, "a");
    str8_list_pushf(arena, &list, "bcd");

    {
      Str8List sub = str8_list_substr(arena, list, 2, 3);
      Str8 res = str8_list_join(arena, &sub, 0);
      TEST_CHECK_(str8_match(res, s("c"), 0), "%s matches c", res.str);
    }

    {
      Str8List sub = str8_list_substr(arena, list, 1, 2);
      Str8 res = str8_list_join(arena, &sub, 0);
      TEST_CHECK_(str8_match(res, s("b"), 0), "%s matches b", res.str);
    }
  }

  {
    TEST_CASE("list substr 3");
    Str8List list = {0};
    str8_list_pushf(arena, &list, "ab");
    str8_list_pushf(arena, &list, "cd");
    str8_list_pushf(arena, &list, "ef");

    {
      Str8List sub = str8_list_substr(arena, list, 1, 5);
      Str8 res = str8_list_join(arena, &sub, 0);
      TEST_CHECK_(str8_match(res, s("bcde"), 0), "%s mathces bcde", res.str);
    }
  }

  {
    TEST_CASE("list substr edge cases");
    Str8List list = {0};
    str8_list_pushf(arena, &list, "abc");

    Str8List zero = str8_list_substr(arena, list, 0, 0);
    TEST_CHECK(memcmp(&zero, &zero_list, sizeof(zero)) == 0);

    Str8List out_of_bounds_range =
        str8_list_substr(arena, list, UINT64_MAX / 2, UINT64_MAX);
    TEST_CHECK(memcmp(&out_of_bounds_range, &zero_list, sizeof(zero_list)) == 0);
  }
  scratch_end(scratch);
}

void test_str8_match_wildcard(void) {
  {
    TEST_CASE("empty strings");
    TEST_CHECK(str8_match_wildcard(s(""), s(""), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("**"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("?"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("*?"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("?*"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit(""), 0) == false);
  }

  {
    TEST_CASE("exact");
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("a"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("A"), 0) == false);
  }

  {
    TEST_CASE("?");
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("?"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("?"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("ab"), str8_lit("?"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("ab"), str8_lit("a?"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("ab"), str8_lit("?b"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("ab"), str8_lit("??"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("a?c"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("ab"), str8_lit("???"), 0) == false);
  }

  {
    TEST_CASE("*");
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("a*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("*c"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("*b*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("a*c"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("b*"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("**"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("a**c"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("a*b*c"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("*a*d*"), 0) == false);
  }

  {
    TEST_CASE("* hard");
    TEST_CHECK(str8_match_wildcard(str8_lit("abcd"), str8_lit("a*d"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abefcdgiescdfimde"), str8_lit("ab*cd?i*de"),
                                   0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("mississippi"), str8_lit("m*iss*ppi"), 0) ==
               1);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("*b"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("aa"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("aa"), str8_lit("a"), 0) == false);
  }

  {
    TEST_CASE("case insensitive");
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("A"),
                                   StringMatchFlag_CaseInsensitive) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("FooBar"), str8_lit("foobar"),
                                   StringMatchFlag_CaseInsensitive) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("Foobar"), str8_lit("foo*"),
                                   StringMatchFlag_CaseInsensitive) == true);
  }

  {
    TEST_CASE("right side sloppy");
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("ab"),
                                   StringMatchFlag_RightSideSloppy) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit(""),
                                   StringMatchFlag_RightSideSloppy) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("a"),
                                   StringMatchFlag_RightSideSloppy) == false);
  }

  {

    TEST_CASE("slash insensitive");
    TEST_CHECK(str8_match_wildcard(str8_lit("a/b"), str8_lit("a\\b"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("a/b"), str8_lit("a\\b"),
                                   StringMatchFlag_SlashInsensitive) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("a/b/c"), str8_lit("a\\*\\c"),
                                   StringMatchFlag_SlashInsensitive) == true);
  }

  {
    TEST_CASE("combined");
    TEST_CHECK(str8_match_wildcard(str8_lit("Ab\\Cde"), str8_lit("ab/*e"),
                                   StringMatchFlag_CaseInsensitive |
                                       StringMatchFlag_SlashInsensitive) == true);

    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("*?*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("*?*"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit(""), str8_lit("*?*"), 0) == false);
    TEST_CHECK(str8_match_wildcard(str8_lit("abc"), str8_lit("?*?"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("ab"), str8_lit("?*?"), 0) == true);
    TEST_CHECK(str8_match_wildcard(str8_lit("a"), str8_lit("?*?"), 0) == false);
  }
}

// clang-format off
TEST_LIST = {
  {"str8_list_substr", test_str8_list_substr},
  {"str8_match_wildcard", test_str8_match_wildcard},
  {NULL, NULL}
};
// clang-format on
