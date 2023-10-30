#pragma once
#include "tools.hpp"
#include <cassert>
#include <cstdlib>
#include <vector>
// #include "basic_treap.hpp"
// #include <bits/stdc++.h>
// using namespace std;

TREAP_CONSTEXPR_AFTER_CXX17
void constexpr_assert(bool b) {
  if (not b) {
#if defined(__has_feature) && __has_feature(address_sanitizer) ||              \
    defined(__SANITIZE_ADDRESS__)
    std::vector<int> v(1);
    for (size_t q = 0;q<(1LLU<<60); ++q){
      v.push_back(v[v.size()]); // cause asan to display stack trace
    }
#endif
    assert(false);
  }
}

template <size_t n> struct test_case {
  constexpr static void (*test_func)() = nullptr;
};

#define TEST_CASE(func)                                                        \
                                                                               \
  TREAP_CONSTEXPR_AFTER_CXX17                                                  \
  void func();                                                                 \
                                                                               \
  template <> struct test_case<__COUNTER__> {                                  \
    constexpr static void (*test_func)() = func;                               \
  };                                                                           \
                                                                               \
  TREAP_CONSTEXPR_AFTER_CXX17                                                  \
  void func()

// RUNNING TESTS

template <size_t start, size_t stop>
TREAP_CONSTEXPR_AFTER_CXX17 bool run_all_tests(bool not_run = false) {
  if (not_run) {
    return false;
  }
  if TREAP_CONSTEXPR_AFTER_CXX17 (start == stop) {
    return true;
  } else {
    if (test_case<start>::test_func) {
      test_case<start>::test_func();
    }
    return run_all_tests<(start + 1) % (stop + 1), stop>();
  }
}

#define RUN_ALL_TESTS() run_all_tests<0, __COUNTER__>()