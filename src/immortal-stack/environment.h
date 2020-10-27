/**
 * @file
 * @brief Header with helper macros that depend on environment (OS, bitness, etc)
 */
#ifndef IMMORTAL_STACK_ENVIRONMENT_H
#define IMMORTAL_STACK_ENVIRONMENT_H

#include <cinttypes>
#include <cstring>

#if _WIN32 || _WIN64
    #if _WIN64
        #define ENV64BIT
    #else
        #define ENV32BIT
    #endif
#endif

#if __GNUC__
    #if __x86_64__ || __ppc64__
        #define ENV64BIT
    #else
        #define ENV32BIT
    #endif
#endif

#if defined(ENV64BIT)
    #define PTR_FORMAT "0x%016" PRIXPTR
#elif defined(ENV32BIT)
    #define PTR_FORMAT "0x%08" PRIXPTR
#endif

#if _WIN32 || _WIN64
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#else
    #define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#endif // IMMORTAL_STACK_ENVIRONMENT_H
