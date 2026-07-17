#ifndef _H_BASE_DEFS
#define _H_BASE_DEFS

#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <stdalign.h>
#include <string.h>

#if defined(_WIN32)
#define OS_WINDOWS 1
#elif defined(__gnu__linux) || defined(__linux__)
#define OS_LINUX 1
#elif defined(__APPLE__) || defined(__MACH__)
#define OS_MAC 1
#endif

#if defined(__clang__)
#define COMPILER_CLANG 1
#elif defined(__MSC_VER)
#define COMPILER_MSVC
#elif defined(__GNUC__) || defined(__GNUG__)
#define COMPILER_GCC
#endif

#define global static
#define internal static
#define local_persist static

#define PI32 3.1415926536f

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define clamp(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))

#define array_count(arr) (sizeof(arr) / sizeof(arr[0]))

#define align_pow_2(x, b) (((x) + (b) - 1) & (~((b) - 1)))

#define memory_zero(s, z) memset((s), 0, (z));

// clang-format off
#define KB(val) ((val)   * 1024LL)
#define MB(val) (KB(val) * 1024LL)
#define GB(val) (MB(val) * 1024LL)
#define TB(val) (GB(val) * 1024LL)
// clang-format on

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef uint64_t usize;

typedef float f32;
typedef double f64;

#endif // _H_BASE_DEFS
