#include "../include/File.h"

#if defined(VKR_WIN32)
#include <windows.h>
#else
#define MAX_PATH 65536
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
