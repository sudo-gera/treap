#include "basic_treap.hpp"

template <typename T> struct ImplicitKeyTreap : BasicTreap<T> {
  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using Iter = typename BasicTreap<T>::Iter;
  using Node = typename BasicTreap<T>::Node;
  using iterator = Iter;
  using const_iterator = Iter;
  using reverse_iterator = std::reverse_iterator<Iter>;
  using const_reverse_iterator = const std::reverse_iterator<Iter>;
  using BasicTreap<T>::size;
  using BasicTreap<T>::clear;
  using BasicTreap<T>::begin;
  using BasicTreap<T>::end;
  using BasicTreap<T>::swap;
  using BasicTreap<T>::root_;
  using BasicTreap<T>::BasicTreap;

private:
  TREAP_CONSTEXPR_AFTER_CXX17
  auto split_predicate(size_t n, bool side) {
    return [n, side](typename BasicTreap<T>::Node *t) mutable {
      auto child = t->get_child(side);
      auto child_size = child ? child->size_ : 0;
      if (n >= child_size + 1) {
        n -= child_size + 1;
        return true;
      }
      return false;
    };
  }

public:
  TREAP_CONSTEXPR_AFTER_CXX17
  friend ImplicitKeyTreap operator|(ImplicitKeyTreap &left, size_t n) {
    return left | left.split_predicate(n, 1);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  friend ImplicitKeyTreap operator|(size_t n, ImplicitKeyTreap &right) {
    return right.split_predicate(n, 0) | right;
  }

  template <typename... CON_ARGS>
  TREAP_CONSTEXPR_AFTER_CXX17 iterator emplace(const_iterator pos,
                                               CON_ARGS &&...args) {
    ImplicitKeyTreap t;
    t.rand_ = this->rand_;
    t.emplace_root(std::forward<CON_ARGS>(args)...);
    this->rand_ = t.rand_;
    auto iter = t.begin();
    (pos - begin() | *this) >> t >> *this;
    return iter;
  }

  template <typename... CON_ARGS>
  TREAP_CONSTEXPR_AFTER_CXX17 const_reference emplace_back(CON_ARGS &&...args) {
    return *emplace(end(), std::forward<CON_ARGS>(args)...);
  }

  template <typename... CON_ARGS>
  TREAP_CONSTEXPR_AFTER_CXX17 const_reference
  emplace_front(CON_ARGS &&...args) {
    return *emplace(begin(), std::forward<CON_ARGS>(args)...);
  }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 void push_back(Y &&val) {
    emplace_back(std::forward<Y>(val));
  }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 void push_front(Y &&val) {
    emplace_front(std::forward<Y>(val));
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  void pop_front() { 1 | *this; }

  TREAP_CONSTEXPR_AFTER_CXX17
  void pop_back() { *this | 1; }

private:
  template <typename... CON_ARGS>
  TREAP_CONSTEXPR_AFTER_CXX17 void va_resize(size_t new_size,
                                             CON_ARGS &&...args) {
    if (size() > new_size) {
      *this | size() - new_size;
      return;
    }
    while (new_size > size()) {
      emplace_back(std::forward<CON_ARGS>(args)...);
    }
  }

public:
  TREAP_CONSTEXPR_AFTER_CXX17
  void resize(size_t new_size) { va_resize(new_size); }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 void resize(size_t new_size, Y &&val) {
    va_resize(new_size, std::forward<Y>(val));
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  iterator erase(const_iterator pos) { return erase(pos, pos + 1); }

  TREAP_CONSTEXPR_AFTER_CXX17
  iterator erase(const_iterator first, const_iterator last) {
    if (last == end()) {
      *this | last - first;
      return end();
    }
    ImplicitKeyTreap tmp;
    (first - begin() | *this) >> tmp << (*this | end() - last);
    swap(tmp);
    return last;
  }

private:
  template <typename INSERT_FUNC>
  TREAP_CONSTEXPR_AFTER_CXX17 iterator insert_template(const_iterator pos,
                                                       INSERT_FUNC &&func) {
    auto tmp = *this | end() - pos;
    size_t old_len = size();
    func();
    auto iter = size() == old_len ? pos : begin() + old_len;
    *this << tmp;
    return iter;
  }

public:
  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 iterator insert(const_iterator pos, Y &&val) {
    return insert_template(pos, [&] { push_back(std::forward<Y>(val)); });
  }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 iterator insert(const_iterator pos, size_t len,
                                              Y &&val) {
    return insert_template(pos,
                           [&] { resize(size() + len, std::forward<Y>(val)); });
  }

  template <typename INPUT_ITER>
  TREAP_CONSTEXPR_AFTER_CXX17 iterator insert(const_iterator pos,
                                              INPUT_ITER first,
                                              INPUT_ITER last) {
    return insert_template(pos, [&] {
      for (; first != last; ++first) {
        emplace_back(*first);
      }
    });
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  iterator insert(const_iterator pos, std::initializer_list<T> list) {
    return insert(pos, list.begin(), list.end());
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  void shrink_to_fit() {}

  TREAP_CONSTEXPR_AFTER_CXX17
  size_t max_size() const { return -1LLU; }

  TREAP_CONSTEXPR_AFTER_CXX17
  const T &operator[](size_t n) const { return begin()[n]; }

  TREAP_CONSTEXPR_AFTER_CXX17
  const T &at(size_t n) const {
    if (n >= size()) {
      throw std::out_of_range("treap::at");
    }
    return begin()[n];
  }

  template <typename INPUT_ITER>
  TREAP_CONSTEXPR_AFTER_CXX17 void assign(INPUT_ITER first, INPUT_ITER last) {
    clear();
    insert(begin(), first, last);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  void assign(const_iterator pos, std::initializer_list<T> list) {
    clear();
    insert(begin(), list);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  auto &operator=(std::initializer_list<T> list) { assign(list); }

  TREAP_CONSTEXPR_AFTER_CXX17
  ImplicitKeyTreap(size_t len) {
    clear();
    resize(len);
  }

  template <typename Y = T>
  TREAP_CONSTEXPR_AFTER_CXX17 ImplicitKeyTreap(size_t len, Y &&value) {
    clear();
    resize(len, std::forward<Y>(value));
  }

  template <typename INPUT_ITER>
  TREAP_CONSTEXPR_AFTER_CXX17 ImplicitKeyTreap(INPUT_ITER first,
                                               decltype((*first, first)) last) {
    assign(first, last);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  ImplicitKeyTreap(std::initializer_list<T> list) { assign(list); }
};
