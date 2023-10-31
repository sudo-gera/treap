#include "implicit_key_treap.hpp"
#include "test_tools.hpp"
#include <bits/stdc++.h>
using namespace std;

struct item {
  size_t value = 0;
  constexpr item(size_t val = 0) : value(val) {}
  constexpr item(item &&) = delete;
  constexpr item(const item &) = default;
  //   item& operator=(item&&) = default;
  //   item& operator=(const item&) = default;
  TREAP_CONSTEXPR_AFTER_CXX17
  static void make(void *) {}
  TREAP_CONSTEXPR_AFTER_CXX17
  static void update(void *) {}
  template <typename NODE, typename OSTREAM>
  TREAP_CONSTEXPR_AFTER_CXX17 static void to_string(NODE *node, OSTREAM &ss) {
    ss << node->value_.value;
  }
  TREAP_CONSTEXPR_AFTER_CXX17
  operator size_t() const { return value; }
};

TEST_CASE(split) {
  ImplicitKeyTreap<item> a, s;
  a.emplace_root(111);
  s.emplace_root(222);
  a << s;
  constexpr_assert(a.size() == 2);
  constexpr_assert(s.size() == 0);
  s = a | 1;
  constexpr_assert(a.size() == 1);
  constexpr_assert(s.size() == 1);
  constexpr_assert(a.root()->value_ == 111);
  constexpr_assert(s.root()->value_ == 222);
  a >> s;
  constexpr_assert(a.size() == 0);
  constexpr_assert(s.size() == 2);
  a = 1 | s;
  constexpr_assert(a.size() == 1);
  constexpr_assert(s.size() == 1);
  constexpr_assert(a.root()->value_ == 111);
  constexpr_assert(s.root()->value_ == 222);
}

template <typename T> TREAP_CONSTEXPR_AFTER_CXX17 void check(T q, T e) {
  std::vector<T> a;
  for (auto w = q; w != e; ++w) {
    a.push_back(w);
  }
  a.push_back(e);
  for (size_t w = 0; w < a.size(); ++w) {
    for (size_t e = 0; e < a.size(); ++e) {
      long d = long(w) - long(e);
      constexpr_assert(a[w] - a[e] == d);
      constexpr_assert(w >= e or a[w] < a[e]);
      constexpr_assert(w <= e or a[w] > a[e]);
      constexpr_assert(w == e or a[w] != a[e]);
      constexpr_assert(w > e or a[w] <= a[e]);
      constexpr_assert(w < e or a[w] >= a[e]);
      constexpr_assert(w != e or a[w] == a[e]);
      constexpr_assert(a[w] + (-d) == a[e]);
      constexpr_assert(a[w] - (d) == a[e]);
    }
  }
  auto s = a;
  s.clear();
  for (auto w = e; w != q; --w) {
    s.push_back(w);
  }
  s.push_back(q);
  reverse(s.begin(), s.end());
  constexpr_assert(a == s);
}

template <typename T> struct constexpr_deque {
  std::array<std::vector<T>, 2> data_;
  TREAP_CONSTEXPR_AFTER_CXX17
  constexpr_deque() = default;
  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 constexpr_deque(size_t len, Y &&val) {
    data_[1].resize(len, std::forward<Y>(val));
  }
  TREAP_CONSTEXPR_AFTER_CXX17

  size_t size() const { return data_[0].size() + data_[1].size(); }
  TREAP_CONSTEXPR_AFTER_CXX17

  void clear() {
    data_[0].clear();
    data_[1].clear();
  }
  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 void push_back(Y &&val) {
    data_[1].push_back(std::forward<Y>(val));
  }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 void push_front(Y &&val) {
    data_[0].push_back(std::forward<Y>(val));
  }
  TREAP_CONSTEXPR_AFTER_CXX17

  void pop_back() {
    if (data_[1].empty()) {
      data_[1].swap(data_[0]);
      reverse(data_[1].begin(), data_[1].end());
    }
    data_[1].pop_back();
  }
  TREAP_CONSTEXPR_AFTER_CXX17

  void pop_front() {
    if (data_[0].empty()) {
      data_[0].swap(data_[1]);
      reverse(data_[0].begin(), data_[0].end());
    }
    data_[0].pop_back();
  }

  struct iterator {
    typename std::vector<T>::iterator iter;
    bool index = 0;
    constexpr_deque *self = 0;
    TREAP_CONSTEXPR_AFTER_CXX17
    auto &operator+=(std::ptrdiff_t val) {
      bool dir = 0;
      if (val < 0) {
        dir = 1;
        val = -val;
      }
      if (index != dir) {
        iter += val;
      } else {
        std::ptrdiff_t diff = iter - self->data_[dir].begin();
        if (val <= diff) {
          iter -= val;
        } else {
          iter = self->data_[not dir].begin();
          iter += val - diff - 1;
          index = not index;
        }
      }
      return *this;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    auto &operator-=(std::ptrdiff_t val) { return (*this) += -val; }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend auto operator+(iterator left, std::ptrdiff_t right) {
      return left += right;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend auto operator-(iterator left, std::ptrdiff_t right) {
      return left -= right;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend auto operator+(std::ptrdiff_t left, iterator right) {
      return right += left;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    std::ptrdiff_t pos() const {
      auto res = iter - self->data_[index].begin() + index;
      if (not index) {
        res = -res;
      }
      return res;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend std::ptrdiff_t operator-(iterator left, iterator right) {
      return left.pos() - right.pos();
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    T &operator*() { return *iter; }
    TREAP_CONSTEXPR_AFTER_CXX17

    const T &operator*() const { return *iter; }
    TREAP_CONSTEXPR_AFTER_CXX17

    T &operator[](std::ptrdiff_t val) { return *(this[0] + val); }
    TREAP_CONSTEXPR_AFTER_CXX17

    const T &operator[](std::ptrdiff_t val) const { return *(this[0] + val); }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend bool operator<(iterator left, iterator right) {
      return left - right < 0;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend bool operator>(iterator left, iterator right) {
      return left - right > 0;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend bool operator<=(iterator left, iterator right) {
      return left - right <= 0;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend bool operator>=(iterator left, iterator right) {
      return left - right >= 0;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend bool operator==(iterator left, iterator right) {
      return left - right == 0;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    friend bool operator!=(iterator left, iterator right) {
      return left - right != 0;
    }
    TREAP_CONSTEXPR_AFTER_CXX17

    auto &operator++() { return (*this) += 1; }
    TREAP_CONSTEXPR_AFTER_CXX17

    auto &operator--() { return (*this) -= 1; }
  };
  TREAP_CONSTEXPR_AFTER_CXX17
  T &operator[](std::ptrdiff_t val) { return begin()[val]; }
  TREAP_CONSTEXPR_AFTER_CXX17

  const T &operator[](std::ptrdiff_t val) const { return begin()[val]; }
  TREAP_CONSTEXPR_AFTER_CXX17

  iterator begin() { return end() - size(); }
  TREAP_CONSTEXPR_AFTER_CXX17

  iterator end() { return {data_[1].end(), 1, this}; }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 void insert(iterator pos, Y &&val) {
    if (pos.index) {
      data_[1].insert(pos.iter, std::forward<Y>(val));
    } else {
      data_[0].insert(pos.iter + 1, std::forward<Y>(val));
    }
  }
  TREAP_CONSTEXPR_AFTER_CXX17

  void erase(iterator pos) {
    if (pos.index) {
      data_[1].erase(pos.iter);
    } else {
      data_[0].erase(pos.iter);
    }
  }
};

TEST_CASE(bulk_all) {
  size_t seed = 1299881216;

  seed = constexpr_seed();

  {
    std::vector<ImplicitKeyTreap<item>> a_vec(1);
    std::vector<constexpr_deque<size_t>> s_vec(1);
    ImplicitKeyTreap<item> *a = a_vec.data();
    constexpr_deque<size_t> *s = s_vec.data();
    size_t q, w, i, v, e;
    constexpr_deque<size_t> tmp;
    FastRand rand{seed};
    for (q = 0; q < 64; ++q) {
      constexpr_assert(a - &a_vec[0] == s - &s_vec[0]);
      constexpr_assert(a->size() == s->size());
      constexpr_assert(a_vec.size() == s_vec.size());
      w = rand() % 14;
      i = rand();
      v = rand();
      if (w == 0) {
        i %= (a->size() + 1);
        a->push_back(v);
        s->push_back(v);
      }
      if (w == 1) {
        i %= (a->size() + 1);
        a->push_front(v);
        s->push_front(v);
      }
      if (w == 2) {
        i %= (a->size() + 1);
        a->insert(i + a->begin(), v);
        s->insert(i + s->begin(), v);
      }
      if (w == 3 and a->size()) {
        i %= (a->size() + 1);
        a->pop_back();
        s->pop_back();
      }
      if (w == 4 and a->size()) {
        i %= (a->size() + 1);
        a->pop_front();
        s->pop_front();
      }
      if (w == 5 and a->size()) {
        i %= a->size();
        a->erase(a->begin() + i);
        s->erase(s->begin() + i);
      }
      if (w == 6) {
        i %= a_vec.size();
        a = &a_vec[i];
        s = &s_vec[i];
      }
      if (w == 7) {
        i %= (a_vec.size() + 1);
        a_vec.emplace_back();
        s_vec.emplace_back();
        a = &a_vec[i];
        s = &s_vec[i];
      }
      if (w == 8) {
        i %= (a_vec.size() + 1);
        a_vec.emplace_back(*a);
        s_vec.emplace_back(*s);
        a = &a_vec[i];
        s = &s_vec[i];
      }
      if (w == 9) {
        i %= (a_vec.size() + 1);
        a_vec.emplace_back(i, v);
        s_vec.emplace_back(i, v);
        a = &a_vec[i];
        s = &s_vec[i];
      }
      if (w == 10) {
        i %= (a_vec.size());
        v %= (a_vec.size());
        a_vec[i] = a_vec[v];
        s_vec[i] = s_vec[v];
        a = &a_vec[i];
        s = &s_vec[i];
      }
      if (w == 11) {
        i %= (a_vec.size());
        v %= (a_vec.size());
        a_vec[i] = std::move(a_vec[v]);
        tmp = std::move(s_vec[v]);
        s_vec[i] = std::move(tmp);
        s_vec[v].clear();
        for (auto &w : a_vec[v]) {
          s_vec[v].push_back(w);
        }
        a = &a_vec[i];
        s = &s_vec[i];
      }
      if (w == 12 and a->size()) {
        i %= (a->size());
        constexpr_assert(((*a)[i]) == ((*s)[i]));
      }
      constexpr_assert(a_vec.size() == s_vec.size());
      constexpr_assert(a->size() == s->size());
      check(s->begin(), s->end());
      check(a->begin(), a->end());
      for (e = 0; e < a->size(); ++e) {
        constexpr_assert((*a)[e] == (*s)[e]);
      }
      constexpr_assert(a->end() - a->begin() == (ssize_t)a->size());
    }
  }
}

#if TREAP_STD_VER > 17
static_assert(RUN_ALL_TESTS());
#endif

int main() { RUN_ALL_TESTS(); }
