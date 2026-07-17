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

// linked list stuff
#define _check_nil(nil, p) ((p) == 0 || (p) == nil)
#define _set_nil(nil, p) ((p) = nil)

// doubly-linked-lists
#define dll_insert_npz(nil, f, l, p, n, next, prev)                                      \
  (_check_nil(nil, f) ? ((f) = (l) = (n), _set_nil(nil, (n)->next),                      \
                         _set_nil(nil, (n)->prev)) /* empty list */                      \
   : _check_nil(nil, p)                            /* p is nil -> insert at front */     \
       ? ((n)->next = (f), (f)->prev = (n), (f) = (n), _set_nil(nil, (n)->prev))         \
       : ((p) == (l)) /* pos == last, push back*/                                        \
             ? ((l)->next = (n), (n)->prev = (l), (l) = (n), _set_nil(nil, (n)->next))   \
             : (((!_check_nil(nil, p) && _check_nil(nil, (p)->next))                     \
                     ? (0)                                                               \
                     : ((p)->next->prev = (n))),                                         \
                ((n)->next = (p)->next), ((p)->next = (n)),                              \
                ((n)->prev = (p)))) /* p is somwhere in the middle */

#define dll_push_back_npz(nil, f, l, n, next, prev)                                      \
  dll_insert_npz(nil, f, l, l, n, next, prev)

#define dll_push_front_npz(nil, f, l, n, next, prev)                                     \
  dll_insert_npz(nil, l, f, f, n, prev, next)

#define dll_remove_npz(nil, f, l, n, next, prev)                                         \
  (((n) == (f) ? (f) = (n)->next : (0)), /* n is head */                                 \
   ((n) == (l) ? (l) = (l)->prev : (0)), /* n is last */                                 \
   (_check_nil(nil, (n)->prev)                                                           \
        ? (0)                                                                            \
        : ((n)->prev->next = (n)->next)), /* bridge over n left side*/                   \
   (_check_nil(nil, (n)->next)                                                           \
        ? (0)                                                                            \
        : ((n)->next->prev = (n)->prev))) /* bridge over n right side*/

// singly-linked, doubly-headed lists (queues)
#define sll_queue_push_nz(nil, f, l, n, next)                                            \
  (_check_nil(nil, f) ? ((f) = (l) = (n), _set_nil(nil, (n)->next))                      \
                      : ((l)->next = (n), (l) = (n), _set_nil(nil, (n)->next)))

#define sll_queue_push_front_nz(nil, f, l, n, next)                                      \
  (_check_nil(nil, f) ? ((f) = (l) = (n), _set_nil(nil, (n)->next))                      \
                      : ((n)->next = (f), (f) = (n)))

#define sll_queue_pop_nz(nil, f, l, next)                                                \
  ((f) == (l) ? (_set_nil(nil, f), _set_nil(nil, l)) : ((f) = (f)->next))

// singly-linked, singly-headed lists (stacks)
#define sll_stack_push_n(f, n, next) ((n)->next = (f), (f) = (n))
#define sll_stack_pop_n(f, next) ((f) = (f)->next)

// doubly-linked-list helpers
#define dll_insert_np(f, l, p, n, next, prev) dll_insert_npz(0, f, l, p, n, next, prev)
#define dll_push_back_np(f, l, n, next, prev) dll_push_back_npz(0, f, l, n, next, prev)
#define dll_push_front_np(f, l, n, next, prev) dll_push_front_npz(0, f, l, n, next, prev)
#define dll_remove_np(f, l, n, next, prev) dll_remove_npz(0, f, l, n, next, prev)
#define dll_insert(f, l, p, n) dll_insert_npz(0, f, l, p, n, next, prev)
#define dll_push_back(f, l, n) dll_push_back_npz(0, f, l, n, next, prev)
#define dll_push_front(f, l, n) dll_push_front_npz(0, f, l, n, next, prev)
#define dll_remove(f, l, n) dll_remove_npz(0, f, l, n, next, prev)

// singly-linked, doubly-headed list helpers
#define sll_queue_push_n(f, l, n, next) sll_queue_push_nz(0, f, l, n, next)
#define sll_queue_push_front_n(f, l, n, next) sll_queue_push_front_nz(0, f, l, n, next)
#define sll_queue_pop_n(f, l, next) sll_queue_pop_nz(0, f, l, next)
#define sll_queue_push(f, l, n) sll_queue_push_nz(0, f, l, n, next)
#define sll_queue_push_front(f, l, n) sll_queue_push_front_nz(0, f, l, n, next)
#define sll_queue_pop(f, l) sll_queue_pop_nz(0, f, l, next)

// singly-linked, singly-headed list helpers
#define sll_stack_push(f, n) sll_stack_push_n(f, n, next)
#define sll_stack_pop(f) sll_stack_pop_n(f, next)

#endif // _H_BASE_DEFS
