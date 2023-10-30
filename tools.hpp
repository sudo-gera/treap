#pragma once
#include <cstddef>

#ifndef TREAP_STD_VER
#if __cplusplus <= 201103L
#define TREAP_STD_VER 11
#elif __cplusplus <= 201402L
#define TREAP_STD_VER 14
#elif __cplusplus <= 201703L
#define TREAP_STD_VER 17
#elif __cplusplus <= 202002L
#define TREAP_STD_VER 20
#else
#define TREAP_STD_VER 23
#endif
#endif // TREAP_STD_VER

#if TREAP_STD_VER > 11
#define TREAP_CONSTEXPR_AFTER_CXX11 constexpr
#else
#define TREAP_CONSTEXPR_AFTER_CXX11
#endif

#if TREAP_STD_VER > 14
#define TREAP_CONSTEXPR_AFTER_CXX14 constexpr
#else
#define TREAP_CONSTEXPR_AFTER_CXX14
#endif

#if TREAP_STD_VER > 17
#define TREAP_CONSTEXPR_AFTER_CXX17 constexpr
#else
#define TREAP_CONSTEXPR_AFTER_CXX17
#endif

struct FastRand {
  std::size_t state = 0x123456;
  TREAP_CONSTEXPR_AFTER_CXX17 std::size_t operator()() {
    state = state << 3 ^ state >> 11;
    state = state >> 5 ^ state << 13;
    state = state << 7 ^ state >> 17;
    return state;
  }
};
