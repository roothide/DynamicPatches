#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslimits.h>
#include <roothide.h>
#include <sys/stat.h>
#include "dobby.h"

struct __CFString {
    uint64_t base[2];
    const char *buffer;
    uint32_t length;
};
