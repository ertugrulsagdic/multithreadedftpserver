/* win32.h */

#if defined(WIN32)
#include <io.h>
#define read  _read
#define write _write
#define STDIN_FILENO  0
#define STDOUT_FILENO 1
#endif /* defined(WIN32) */
