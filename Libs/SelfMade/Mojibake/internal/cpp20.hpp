#pragma once

#if __cplusplus >= 202002L
  // consteval is C++20
  // constinit is C++20
#else
  #define consteval constexpr
  #define constinit
#endif
