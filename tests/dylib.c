#include "bace/bace.h"
#include "bace/base_os.h"

#define TEST_INIT bace_os_state_init();
#include "acutest.h"

void test_dylib_basic(void) {
#if OS_WINDOWS
  DyLib lib = lib_open(s("kernel32.dll"));
  TEST_CHECK(lib.handle != 0);
  {
    TEST_CASE("GetTickCount");
    VoidProc *proc = lib_load_proc(lib, s("GetTickCount"));
    TEST_CHECK(proc != NULL);
    DWORD res = ((DWORD(WINAPI *)(void))proc)();
    TEST_CHECK_(res != 0, "%lu is not zero", res);
  }

  {
    TEST_CASE("lstrlenA");
    Str8 fname = s("lstrlenA");
    VoidProc *proc = lib_load_proc(lib, fname);
    TEST_CHECK(proc != NULL);
    int res = ((int(WINAPI *)(LPCSTR))proc)((LPCSTR)fname.str);
    TEST_CHECK_((u64)res == fname.size, "%d is len of %s", res, fname.str);
  }
#elif OS_LINUX
  DyLib lib = lib_open(s("libc.so.6"));
  TEST_CHECK(lib.handle != 0);
  {
    TEST_CASE("time");
    VoidProc *proc = lib_load_proc(lib, s("time"));
    TEST_CHECK(proc != NULL);
    time_t res = ((time_t (*)(time_t *))proc)(0);
    TEST_CHECK_(res > 0, "%lu is greater than zero", res);
  }

  {
    TEST_CASE("strlen");
    Str8 fname = s("strlen");
    VoidProc *proc = lib_load_proc(lib, fname);
    TEST_CHECK(proc != NULL);
    size_t res = ((size_t (*)(const char *))proc)((char *)fname.str);
    TEST_CHECK_((u64)res == fname.size, "%zu is len of %s", res, fname.str);
  }
#endif
}

// clang-format off
TEST_LIST = {
  {"dylib_tests", test_dylib_basic},
  {NULL, NULL}
};
// clang-format on
