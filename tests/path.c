#include "bace/strings.h"
#include "bace/bace.h"
#include "bace/thread_context.h"
#include "bace/path.h"

#define TEST_INIT bace_os_state_init()
#include "acutest.h"

void test_str8_slash_chop_skip(void) {
  {
    TEST_CASE("str8_chop_last_slash");
    TEST_CHECK(str8_match(str8_chop_last_slash(s("a/b/c")), s("a/b"), 0));
    TEST_CHECK(str8_match(str8_chop_last_slash(s("a/b/")), s("a/b"), 0));

    TEST_CHECK(str8_match(str8_chop_last_slash(s("abc")), s(""), 0));
    TEST_CHECK(str8_match(str8_chop_last_slash(s("")), s(""), 0));
  }

  {
    TEST_CASE("str8_skip_last_slash");
    TEST_CHECK(str8_match(str8_skip_last_slash(s("a/b/c")), s("c"), 0));
    TEST_CHECK(str8_match(str8_skip_last_slash(s("a/b/")), s(""), 0));

    TEST_CHECK(str8_match(str8_skip_last_slash(s("abc")), s("abc"), 0));
    TEST_CHECK(str8_match(str8_skip_last_slash(s("")), s(""), 0));
  }
}

void test_str8_dot_chop_skip(void) {
  {
    TEST_CASE("str8_chop_last_dot");
    TEST_CHECK(str8_match(str8_chop_last_dot(s("file.txt")), s("file"), 0));
    TEST_CHECK(str8_match(str8_chop_last_dot(s("file.tar.gz")), s("file.tar"), 0));

    TEST_CHECK(
        str8_match(str8_chop_last_dot(s("my.folder/file")), s("my.folder/file"), 0));
    TEST_CHECK(str8_match(str8_chop_last_dot(s("no_ext_file")), s("no_ext_file"), 0));
  }

  {
    TEST_CASE("str8_skip_last_dot");
    TEST_CHECK(str8_match(str8_skip_last_dot(s("file.txt")), s("txt"), 0));
    TEST_CHECK(str8_match(str8_skip_last_dot(s("file.tar.gz")), s("gz"), 0));

    TEST_CHECK(
        str8_match(str8_skip_last_dot(s("my.folder/file")), s("my.folder/file"), 0));
    TEST_CHECK(str8_match(str8_skip_last_dot(s("no_ext_file")), s("no_ext_file"), 0));
  }
}

void test_path_normalization(void) {
  Temp scratch = scratch_begin(0, 0);
  Arena *arena = scratch.arena;

  {
    TEST_CASE("relative paths");
    Str8 res1 = path_normalized_from_string(arena, s("a/b/../c"));
    TEST_CHECK_(str8_match(res1, s("a/c"), 0), "resolves to %s", res1.str);

    Str8 res2 = path_normalized_from_string(arena, s("a/./b/./c"));
    TEST_CHECK_(str8_match(res2, s("a/b/c"), 0), "resolves to %s", res2.str);

    Str8 res3 = path_normalized_from_string(arena, s("../../a/b"));
    TEST_CHECK_(str8_match(res3, s("../../a/b"), 0), "resolves to %s", res3.str);
  }

  {
    TEST_CASE("absolute paths");
    // unix
    Str8 res_unix = path_normalized_from_string(arena, s("/a/b/../c"));
    TEST_CHECK_(str8_match(res_unix, s("/a/c"), 0), "resolves to %s", res_unix.str);

    // windows
    Str8 res_win = path_normalized_from_string(arena, s("C:/a/b/../c"));
    TEST_CHECK_(str8_match(res_win, s("C:/a/c"), 0), "resolves to %s", res_win.str);
  }

  {
    TEST_CASE("double slash");
    Str8 res = path_normalized_from_string(arena, s("a//b///c"));
    TEST_CHECK_(str8_match(res, s("a/b/c"), 0), "resolves to %s", res.str);

    Str8 res_unix = path_normalized_from_string(arena, s("//a/b/"));
    TEST_CHECK_(str8_match(res_unix, s("/a/b"), 0), "resolves to %s", res_unix.str);
  }

  scratch_end(scratch);
}

void test_path_relative_absolute_conversion(void) {
  Temp scratch = scratch_begin(0, 0);
  Arena *arena = scratch.arena;

  {
    TEST_CASE("relative from absolute");
    Str8 dst = s("/project/src/main.c");
    Str8 src = s("/project/");

    Str8 res = path_relative_dst_from_absolute_dst_src(arena, dst, src);
    TEST_CHECK_(str8_match(res, s("src/main.c"), 0), "relative path is %s", res.str);

    Str8 dst2 = s("/project/build/app.exe");
    Str8 src2 = s("/project/src/");

    Str8 res2 = path_relative_dst_from_absolute_dst_src(arena, dst2, src2);
    TEST_CHECK_(str8_match(res2, s("../build/app.exe"), 0), "relative path is %s",
                res2.str);
  }

  {
    TEST_CASE("absolute from relative");
    Str8 rel = s("../build/app.exe");
    Str8 src = s("/project/src/");

    Str8 res = path_absolute_dst_from_relative_dst_src(arena, rel, src);
    TEST_CHECK_(str8_match(res, s("/project/build/app.exe"), 0), "absolute path is %s",
                res.str);
  }

  scratch_end(scratch);
}

void test_path_normalization_ll_regression(void) {
  Temp scratch = scratch_begin(0, 0);
  Arena *arena = scratch.arena;

  {
    TEST_CASE("dangling next pointer on multiple pops");
    Str8 res1 = path_normalized_from_string(arena, s("a/b/c/../../d"));
    TEST_CHECK_(str8_match(res1, s("a/d"), 0), "expected 'a/d', got '%s'", res1.str);

    Str8 res2 = path_normalized_from_string(arena, s("dir1/dir2/dir3/../../../dir4"));
    TEST_CHECK_(str8_match(res2, s("dir4"), 0), "expected 'dir4', got '%s'", res2.str);
  }

  {
    TEST_CASE("leading unstacked nodes overwritten on stack exhaustion");
    // if relative paths started with MULTIPLE `..` nodes, they were
    // saved "without stack".
    // if a later `..` popped the stack back to 0,
    // the old code did `path->last = path->first`, effectively erasing all
    // leading `..` nodes except the very first one.
    Str8 res3 = path_normalized_from_string(arena, s("../../a/../b"));
    TEST_CHECK_(str8_match(res3, s("../../b"), 0), "expected '../../b', got '%s'",
                res3.str);

    Str8 res4 =
        path_normalized_from_string(arena, s("../../../folder/sub/../../file.txt"));
    TEST_CHECK_(str8_match(res4, s("../../../file.txt"), 0),
                "expected '../../../file.txt', got '%s'", res4.str);
    Str8 res4_ =
        path_normalized_from_string(arena, s("../../../folder/sub/../../../file.txt"));
    TEST_CHECK_(str8_match(res4_, s("../../../../file.txt"), 0),
                "expected '../../../../file.txt', got '%s'", res4_.str);
  }

  {
    TEST_CASE("windows drive letter with stack exhaustion");
    // Windows drive letters ("C:") are saved without a stack node.
    // popping back to the root needs to properly anchor to the drive letter without
    // corrupting the links.
    Str8 res5 = path_normalized_from_string(arena, s("C:/foo/bar/../../baz"));
    TEST_CHECK_(str8_match(res5, s("C:/baz"), 0), "expected 'C:/baz', got '%s'",
                res5.str);
  }

  {
    TEST_CASE("empty string segments from triple slashes");
    Str8 res6 = path_normalized_from_string(arena, s("a///b/..///c"));
    TEST_CHECK_(str8_match(res6, s("a/c"), 0), "expected 'a/c', got '%s'", res6.str);
  }
  scratch_end(scratch);
}

// clang-format off
TEST_LIST = {
  {"str8_slash_chop_skip", test_str8_slash_chop_skip},
  {"str8_dot_chop_skip", test_str8_dot_chop_skip},
  {"path_normalization", test_path_normalization},
  {"path_relative_absolute_conversion", test_path_relative_absolute_conversion},
  {"ll_bugs_regression", test_path_normalization_ll_regression},
  {NULL, NULL}
};
// clang-format on
