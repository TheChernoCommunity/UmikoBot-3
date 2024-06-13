#pragma once

#include <stdio.h>

#define UNUSED(x) (void) (x);

// Compiler.
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
    #define UMIKO_COMPILER_GNU_LIKE 1
#elif defined(_MSC_VER)
    #define UMIKO_COMPILER_MSVC 1
#else
    #error "This compiler is not currently supported."
#endif

#if UMIKO_COMPILER_GNU_LIKE
    #define UMIKO_PRINTF_LIKE __attribute__((format(printf, 2, 3)))
#else
    #define UMIKO_PRINTF_LIKE
#endif
