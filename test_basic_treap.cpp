#include "basic_treap.hpp"
#include "test_tools.hpp"
#include <bits/stdc++.h>
using namespace std;

struct item {
  size_t value = 0;
  constexpr item(size_t val = 0) : value(val) {}
  constexpr item(item &&other) = delete;
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

TEST_CASE(constructor_destructor) { [[maybe_unused]] BasicTreap<item> t; }

TEST_CASE(change_size) {
  [[maybe_unused]] BasicTreap<item> t;
  constexpr_assert(t.empty());
  constexpr_assert(t.size() == 0);
  t.clear();
  constexpr_assert(t.empty());
  constexpr_assert(t.size() == 0);
  t.emplace_root();
  constexpr_assert(not t.empty());
  constexpr_assert(t.size() == 1);
  t.emplace_root();
  constexpr_assert(not t.empty());
  constexpr_assert(t.size() == 1);
  t.clear();
  constexpr_assert(t.empty());
  constexpr_assert(t.size() == 0);
  t.emplace_root(999);
  constexpr_assert(not t.empty());
  constexpr_assert(t.size() == 1);
  constexpr_assert(t.root()->value_ == 999);
  t.root()->value_.value = 888;
  constexpr_assert(t.root()->value_.value == 888);
}

TEST_CASE(merge) {
  [[maybe_unused]] BasicTreap<item> t;
  [[maybe_unused]] BasicTreap<item> y;
  t.emplace_root(123);
  y.emplace_root(321);
  t << y;
  constexpr_assert(t.size() == 2);
  constexpr_assert(y.size() == 0);
  constexpr_assert(t.root()->value_ == 123 or t.root()->value_ == 321);
}

TEST_CASE(cmp) {
  [[maybe_unused]] BasicTreap<item> t;
  [[maybe_unused]] BasicTreap<item> y;
  constexpr_assert(t == y);
  constexpr_assert(not(t < y));
  constexpr_assert(not(y < t));
  t.emplace_root(111);
  y.emplace_root(111);
  constexpr_assert(t == y);
  constexpr_assert(not(t < y));
  constexpr_assert(not(y < t));
  y.emplace_root(222);
  constexpr_assert(not(t == y));
  constexpr_assert(t < y);
  constexpr_assert(not(y < t));
  [[maybe_unused]] BasicTreap<item> u;
  u.emplace_root(333);
  u >> y;
  u.emplace_root(444);
  u >> t;
  constexpr_assert(not(t == y));
  constexpr_assert(not(t < y));
  constexpr_assert(y < t);
  u.emplace_root(555);
  u >> t;
  u.emplace_root(666);
  u >> y;
  constexpr_assert(not(t == y));
  constexpr_assert(t < y);
  constexpr_assert(not(y < t));
}

TEST_CASE(iterators) {
  [[maybe_unused]] BasicTreap<item> t;
  [[maybe_unused]] BasicTreap<item> y;
  constexpr_assert(t.begin() == t.end());
  constexpr_assert(not(t.begin() < t.end()));
  constexpr_assert(not(t.begin() > t.end()));
  constexpr_assert(t.begin() - t.end() == 0);
  constexpr_assert(t.end() - t.begin() == 0);
  t.emplace_root(999);
  constexpr_assert(not(t.begin() == t.end()));
  constexpr_assert(t.begin() < t.end());
  constexpr_assert(not(t.begin() > t.end()));
  constexpr_assert(t.begin() - t.end() == -1);
  constexpr_assert(t.end() - t.begin() == 1);
  y.emplace_root(888);
  y >> t;
  constexpr_assert(t.begin() - t.end() == -2);
  constexpr_assert(t.end() - t.begin() == 2);
  y.emplace_root(777);
  y >> t;
  constexpr_assert(t.begin() - t.end() == -3);
  constexpr_assert(t.end() - t.begin() == 3);
}

template <typename T, size_t start> struct call_from_tuple {
  TREAP_CONSTEXPR_AFTER_CXX17
  void call(T &t, size_t n) {
    if (n == start - 1) {
      std::get<start - 1>(t)();
    } else {
      call_from_tuple<T, (start + 1) % (std::tuple_size_v<T> + 1)>().call(t, n);
    }
  }
};

template <typename T> struct call_from_tuple<T, 0> {
  TREAP_CONSTEXPR_AFTER_CXX17
  void call(T &t, size_t n) {}
};

struct null_out {};

template <typename T>
TREAP_CONSTEXPR_AFTER_CXX17 null_out operator<<(null_out, T &&) {
  return {};
}

template <typename T> TREAP_CONSTEXPR_AFTER_CXX17 int cmp(T &&left, T &&right) {
  return (left > right) - (left < right);
}

template <typename OSTREAM>
TREAP_CONSTEXPR_AFTER_CXX17 bool test(bool run, OSTREAM &&out, size_t seed) {
  if (not run) {
    return true;
  }
  vector<pair<BasicTreap<item>, vector<BasicTreap<item>::iterator>>> treaps(1);
  treaps[0].second.push_back(treaps[0].first.begin());
  decltype(treaps)::value_type tp;
  vector<vector<size_t>> vectors(1);
  FastRand rand;
  rand.state = seed;
  size_t op, value, current = 0;
  auto operations = make_tuple(
      [&] {
        treaps.emplace_back();
        vectors.emplace_back();
        auto &treap = treaps.back();
        auto &vector = vectors.back();
        for (size_t q = 0; q < 9; ++q) {
          auto tmp = std::move(treap.first);
          treap.first.emplace_root(value);
          if (value % 2) {
            treap.first << tmp;
          } else {
            tmp >> treap.first;
          }
          value >>= 1;
        }
        for (auto it = treap.first.begin(); it != treap.first.end(); ++it) {
          treap.second.push_back(it);
          vector.push_back(it->value);
        }
        treap.second.push_back(treap.first.end());
        constexpr_assert(treap.first.size() == 9);
      },
      [&] {
        treaps.emplace_back();
        auto &treap = treaps[current];
        vectors.emplace_back();
        auto &vector = vectors[current];
        constexpr_assert(treap.first.size() == vector.size());
        size_t n = value % (treap.first.size() + 1);
        treaps.back().first = [n](decltype(treaps[0].first.root()) t) mutable {
          auto child = t->get_child(0);
          auto child_size = child ? child->size_ : 0;
          if (n >= child_size + 1) {
            n -= child_size + 1;
            return true;
          }
          return false;
        } | treap.first;
        copy(vector.begin(), vector.begin() + n,
             back_insert_iterator<
                 typename std::decay_t<decltype(vectors.back())>>(
                 vectors.back()));
        rotate(vector.begin(), vector.begin() + n, vector.end());
        vector.resize(vector.size() - n);
        n += n + 1 == treap.second.size();
        constexpr_assert(n <= treap.second.size());
        copy(treap.second.begin(), treap.second.begin() + n,
             back_inserter(treaps.back().second));
        rotate(treap.second.begin(), treap.second.begin() + n,
               treap.second.end());
        treap.second.resize(treap.second.size() - n);
        if (treap.second.empty()) {
          treap.second.push_back(treap.first.end());
        } else {
          treaps.back().second.push_back(treaps.back().first.end());
        }
        current = treaps.size() - 1;
      },
      [&] {
        treaps.emplace_back();
        auto &treap = treaps[current];
        vectors.emplace_back();
        auto &vector = vectors[current];
        constexpr_assert(treap.first.size() == vector.size());
        size_t n = value % (treap.first.size() + 1);
        treaps.back().first =
            treap.first | [n](decltype(treaps[0].first.root()) t) mutable {
              auto child = t->get_child(1);
              auto child_size = child ? child->size_ : 0;
              if (n >= child_size + 1) {
                n -= child_size + 1;
                return true;
              }
              return false;
            };
        copy(vector.end() - n, vector.end(), back_inserter(vectors.back()));
        vector.resize(vector.size() - n);
        n += 1;
        constexpr_assert(n <= treap.second.size());
        if (n == 1) {
          treaps.back().second.push_back(treaps.back().first.begin());
        } else {
          copy(treap.second.end() - n, treap.second.end(),
               back_inserter(treaps.back().second));
          treap.second.resize(treap.second.size() - n);
          treap.second.push_back(treap.first.end());
        }
        current = treaps.size() - 1;
      },
      [&] { current = value % treaps.size(); },
      [&] {
        auto index = value % treaps.size();
        constexpr_assert(cmp(vectors[current], vectors[index]) ==
                         cmp(treaps[current].first, treaps[index].first));
      },
      [&] {
        if (treaps.size() > 1) {
          auto index = value % treaps.size();
          index ^= index == current;
          index %= treaps.size();
          constexpr_assert(current < treaps.size());
          constexpr_assert(index < treaps.size());
          treaps[index].first >> treaps[current].first;
          if (treaps[current].second.size() == 1) {
            treaps[current].second = std::move(treaps[index].second);
          } else {
            treaps[index].second.pop_back();
            treaps[current].second.insert(treaps[current].second.begin(),
                                          treaps[index].second.begin(),
                                          treaps[index].second.end());
          }
          treaps[index].second.clear();
          treaps[index].second.push_back(treaps[index].first.begin());
          vectors[current].insert(vectors[current].begin(),
                                  vectors[index].begin(), vectors[index].end());
          vectors[index].clear();
          constexpr_assert(treaps[index].first.size() + 1 ==
                           treaps[index].second.size());
          constexpr_assert(treaps[index].first.size() == vectors[index].size());
          constexpr_assert(treaps[current].first.size() + 1 ==
                           treaps[current].second.size());
          constexpr_assert(treaps[current].first.size() ==
                           vectors[current].size());
          treaps.erase(treaps.begin() + index);
          vectors.erase(vectors.begin() + index);
          current -= current == treaps.size();
        }
      },
      [&] {
        if (treaps.size() > 1) {
          auto index = value % treaps.size();
          index ^= index == current;
          index %= treaps.size();
          constexpr_assert(current < treaps.size());
          constexpr_assert(index < treaps.size());
          treaps[current].first << treaps[index].first;
          if (treaps[current].second.size() == 1) {
            treaps[current].second = std::move(treaps[index].second);
          } else if (treaps[index].second.size() != 1) {
            treaps[current].second.pop_back();
            treaps[current].second.insert(treaps[current].second.end(),
                                          treaps[index].second.begin(),
                                          treaps[index].second.end());
          }
          treaps[index].second.clear();
          treaps[index].second.push_back(treaps[index].first.begin());
          vectors[current].insert(vectors[current].end(),
                                  vectors[index].begin(), vectors[index].end());
          vectors[index].clear();
          constexpr_assert(treaps[index].first.size() + 1 ==
                           treaps[index].second.size());
          constexpr_assert(treaps[index].first.size() == vectors[index].size());
          constexpr_assert(treaps[current].first.size() + 1 ==
                           treaps[current].second.size());
          constexpr_assert(treaps[current].first.size() ==
                           vectors[current].size());
          treaps.erase(treaps.begin() + index);
          vectors.erase(vectors.begin() + index);
          current -= current == treaps.size();
        }
      });
  for (size_t q = 0; q < 2; ++q) {
    op = rand() % (tuple_size_v<decltype(operations)> + 2);
    if (op >= tuple_size_v<decltype(operations)>) {
      op -= 2;
    }
    value = rand();
    out << q << " " << current << " " << op << " " << value << " "
        << treaps.size() << " " << treaps[current].first.size() << "\n";
    call_from_tuple<decltype(operations), 1>().call(operations, op);
    constexpr_assert(vectors.size() == treaps.size());
    for (size_t current = 0; current < treaps.size(); ++current) {
      auto &treap = treaps[current];
      auto &vector = vectors[current];
      constexpr_assert(current < vectors.size());
      constexpr_assert(treap.first.size() == vector.size());
      constexpr_assert(treap.first.size() + 1 == treap.second.size());
      for (size_t q = 0; q < treap.second.size(); ++q) {
        for (size_t w = 0; w < treap.second.size(); ++w) {
          auto d = q - w;
          constexpr_assert(treap.second[q] - treap.second[w] == d);
          constexpr_assert(q >= w or treap.second[q] < treap.second[w]);
          constexpr_assert(q <= w or treap.second[q] > treap.second[w]);
          constexpr_assert(q == w or treap.second[q] != treap.second[w]);
          constexpr_assert(q > w or treap.second[q] <= treap.second[w]);
          constexpr_assert(q < w or treap.second[q] >= treap.second[w]);
          constexpr_assert(q != w or treap.second[q] == treap.second[w]);
          constexpr_assert(treap.second[q] + -d == treap.second[w]);
          constexpr_assert(treap.second[q] - d == treap.second[w]);
        }
      }
      auto it = treap.first.begin();
      for (size_t q = 0; q < vector.size(); ++q) {
        constexpr_assert(it->value == vector[q]);
        constexpr_assert(it == treap.second[q]);
        constexpr_assert(&*it == &*treap.second[q]);
        for (size_t w = 0; w < treap.second.size(); ++w) {
          constexpr_assert(&*it == &treap.second[w][q - w]);
        }
        ++it;
      }
      constexpr_assert(it == treap.first.end());
      constexpr_assert(it == treap.second.back());
    }
  }
  return true;
}

TEST_CASE(bulk_all) { // TODO: examine and fix
  size_t seed = 0x123;
  seed = constexpr_seed();
  auto out = null_out();
  //   auto& out = cout;
  test(1, out, seed);
}

#if TREAP_STD_VER > 17
static_assert(RUN_ALL_TESTS());
#endif

int main() { RUN_ALL_TESTS(); }
