#ifndef _H_BACE
#define _H_BACE

#include "bace/base_os.h"        // IWYU pragma: export
#include "bace/arena.h"          // IWYU pragma: export
#include "bace/strings.h"        // IWYU pragma: export
#include "bace/thread_context.h" // IWYU pragma: export
#include "bace/math.h"           // IWYU pragma: export

#if OS_LINUX
#include "bace/linux/base.h" // IWYU pragma: export
#elif OS_WINDOWS
#include "bace/win32/base.h" // IWYU pragma: export
#endif

#endif // !_H_BACE
