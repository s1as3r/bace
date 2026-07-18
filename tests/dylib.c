#include "bace/bace.h"
#include "bace/base_os.h"

void init_state(void) {
#if OS_LINUX
  init_linux_state();
#elif OS_WINDOWS
  init_win32_state();
#endif
}

#define TEST_INIT init_state()
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
#endif
}

// clang-format off
TEST_LIST = {
  {"dylib_tests", test_dylib_basic},
  {NULL, NULL}
};
// clang-format on
