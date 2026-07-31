// bace.h
//
// including this single file pulls in the whole library.
// convenience header for code that uses most of the library.
// lower-level modules that only need one piece should prefer including that
// module's header directly.

#ifndef _H_BACE
#define _H_BACE

#include "bace/base_os.h"        // IWYU pragma: export
#include "bace/arena.h"          // IWYU pragma: export
#include "bace/strings.h"        // IWYU pragma: export
#include "bace/thread_context.h" // IWYU pragma: export
#include "bace/math.h"           // IWYU pragma: export
#include "bace/time.h"           // IWYU pragma: export
#include "bace/path.h"           // IWYU pragma: export
#include "bace/files.h"          // IWYU pragma: export

#if OS_LINUX
#include "bace/linux/base.h" // IWYU pragma: export
#elif OS_WINDOWS
#include "bace/win32/base.h" // IWYU pragma: export
#endif

#endif // !_H_BACE
