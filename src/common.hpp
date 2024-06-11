#pragma once

#include <stdio.h>

#define UNUSED(x) (void) (x);

// @Incomplete: Proper logging, to a file.
#define umiko_info  printf
#define umiko_error printf
