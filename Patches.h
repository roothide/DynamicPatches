#include <Foundation/Foundation.h>
#include <unistd.h>
#include <roothide.h>
#include <sys/stat.h>
#include "dobby.h"

struct __CFString {
    uint64_t base[2];
    const char *buffer;
    UInt32 length;
};