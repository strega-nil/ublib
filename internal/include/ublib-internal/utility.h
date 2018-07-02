#ifndef _UBLIB_INTERNAL_UTILITY_H
#define _UBLIB_INTERNAL_UTILITY_H

#if !defined(NDEBUG)
#include <iostream>
#endif

#if defined(NDEBUG)

#if defined(__GNUC__)

#define __ublib_undefined_behavior(msg)\
  __builtin_unreachable()

#elif defined(_MSC_VER)

#define __ublib_undefined_behavior(msg)\
  __assume(0)

#else
#define __ublib_undefined_behavior(msg)
#endif

#else

#define __ublib_undefined_behavior(msg)\
  ([] {\
    std::cerr << msg << '\n';\
    std::abort();\
  }())

#endif

#endif