#include "tools.hpp"
#include <array>
#include <cassert>

template <typename T> struct BasicTreap {
  struct Node {
    std::size_t priority_ = 0;
    std::array<Node *, 2> children_ = {nullptr, nullptr};
    Node *parent_ = nullptr;
    std::size_t size_ = 1;
    std::size_t height_ = 1;
    T value_;

    template <typename... CON_ARGS>
    TREAP_CONSTEXPR_AFTER_CXX17 Node(FastRand &rand, CON_ARGS &&...args)
        : priority_(rand()), value_(std::forward<CON_ARGS &&>(args)...) {
      update(this);
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    Node *get_child(bool num) {
      make(children_[num]);
      return children_[num];
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    static void make(Node *root) {
      if (root) {
        T::make(root);
      }
      // check(root);
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    static void update(Node *root) {
      if (root) {
        std::array<std::size_t, 2> children_height = {0, 0};
        std::array<std::size_t, 2> children_size = {0, 0};
        for (std::size_t q = 0; q < 2; ++q) {
          if (root->children_[q]) {
            auto &child = root->children_[q];
            child->parent_ = root;
            children_height[q] = child->height_;
            children_size[q] = child->size_;
          }
        }
        root->height_ =
            1 + (children_height[0] > children_height[1] ? children_height[0]
                                                         : children_height[1]);
        root->size_ = children_size[0] + 1 + children_size[1];
        T::update(root);
      }
      // check(root);
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    static void check(Node *root) {
      if (not root) {
        return;
      }
      assert(root->height_ < 128);
      check(root->children_[0]);
      check(root->children_[1]);
      size_t size[2] = {0, 0};
      size_t height[2] = {0, 0};
      for (size_t q = 0; q < 2; ++q) {
        if (root->children_[q]) {
          Node *child = root->children_[q];
          assert(root == child->parent_);
          size[q] = child->size_;
          height[q] = child->height_;
        }
      }
      assert(size[0] + 1 + size[1] == root->size_);
      assert(std::max(height[0], height[1]) + 1 == root->height_);
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    static void del(Node *q) {
      if (not q) {
        return;
      }
      del(q->children_[0]);
      del(q->children_[1]);
      delete q;
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    static Node *merge(std::array<Node *, 2> roots,
                       bool args_order_is_not_reversed) {
      if (not roots[0]) {
        return roots[1];
      }
      if (not roots[1]) {
        return roots[0];
      }
      assert(roots[0] != roots[1]);
      auto new_root = roots[0]->priority_ < roots[1]->priority_;
      auto child = new_root ^ args_order_is_not_reversed;
      roots[new_root]->children_[child] = merge(
          {roots[new_root]->get_child(child), roots[new_root ^ 1]}, child);
      update(roots[new_root]);
      return roots[new_root];
    }

    template <bool condition_tells_if_is_right = 0, typename COND>
    TREAP_CONSTEXPR_AFTER_CXX17 static std::array<Node *, 2>
    split(Node *root, COND &&condition) {
      if (not root) {
        return {nullptr, nullptr};
      }
      bool is_left = condition_tells_if_is_right ^ bool(condition(root));
      auto child = root->get_child(is_left);
      auto tmp = split<condition_tells_if_is_right>(
          child, std::forward<COND>(condition));
      root->children_[is_left] = tmp[is_left ^ 1];
      update(root);
      tmp[is_left ^ 1] = root;
      if (tmp[is_left]) {
        tmp[is_left]->parent_ = nullptr;
      }
      return tmp;
    }

    // IO

    struct PrintNode {
      std::size_t value_ = 0;
      std::size_t save = 0;
      PrintNode *next = nullptr;
    };

    template <typename OSTREAM>
    TREAP_CONSTEXPR_AFTER_CXX17 static void
    print(Node *root, OSTREAM &ss, PrintNode *start = nullptr,
          PrintNode *prev_prev_node = nullptr) {
      if (not root) {
        return;
      }
      if (not start) {
        PrintNode prev1{3};
        PrintNode prev2{0, 0, &prev1};
        print(root, ss, &prev1, &prev2);
        return;
      }
      prev_prev_node->save = prev_prev_node->value_;
      if (prev_prev_node->value_ == prev_prev_node->next->value_) {
        prev_prev_node->value_ = 0;
      }
      PrintNode node;
      prev_prev_node->next->next = &node;
      node.value_ = 2;
      print(root->children_[0], ss, start, prev_prev_node->next);
      for (PrintNode *n = start; n != &node; n = n->next) {
        char const *strs[] = {" ", " ", "┃", "┗", "┃", "┏", "", ""};
        ss << strs[n->value_ * 2 + (n == prev_prev_node->next)];
      }
      char const *strs[] = {"━", "┳", "┻", "╋"};
      ss << strs[bool(root->children_[0]) * 2 + bool(root->children_[1])]
         << "► ";
      T::to_string(root, ss);
      ss << "\n";
      node.value_ = 1;
      print(root->children_[1], ss, start, prev_prev_node->next);
      prev_prev_node->value_ = prev_prev_node->save;
    }

    // ITER

    TREAP_CONSTEXPR_AFTER_CXX17
    static std::pair<Node *, bool> add(Node *current, std::ptrdiff_t offset) {
      if (not current) {
        return {nullptr, 0};
      }
      auto left = current->get_child(0);
      auto right = current->get_child(1);
      std::size_t left_size = left ? left->size_ : 0;
      std::size_t right_size = right ? right->size_ : 0;
      while (offset and current) {
        if (offset > (std::ptrdiff_t)right_size or
            -offset > (std::ptrdiff_t)left_size) {
          if (not current->parent_) {
            return {current, 1};
            // current = nullptr;
            // break;
          } else if (current == current->parent_->children_[0]) {
            offset -= right_size + 1;
            left = current;
            current = current->parent_;
            right = current->get_child(1);
            left_size = left->size_;
            right_size = right ? right->size_ : 0;
          } else if (current == current->parent_->children_[1]) {
            offset += left_size + 1;
            right = current;
            current = current->parent_;
            left = current->get_child(0);
            left_size = left ? left->size_ : 0;
            right_size = right->size_;
          }
        } else if (offset < 0) {
          current = left;
          right = current->get_child(1);
          right_size = right ? right->size_ : 0;
          left = current->get_child(0);
          left_size = left ? left->size_ : 0;
          offset += right_size + 1;
        } else {
          current = right;
          left = current->get_child(0);
          left_size = left ? left->size_ : 0;
          right = current->get_child(1);
          right_size = right ? right->size_ : 0;
          offset -= left_size + 1;
        }
      }
      return {current, 0};
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    static std::ptrdiff_t root_offset(Node *current) {
      if (not current) {
        return 0;
      }
      std::ptrdiff_t ans = 0;
      while (current->parent_) {
        if (current->parent_->children_[0] == current) {
          auto right = current->get_child(1);
          auto right_size = right ? right->size_ : 0;
          ans -= right_size + 1;
        } else if (current->parent_->children_[1] == current) {
          auto left = current->get_child(0);
          auto left_size = left ? left->size_ : 0;
          ans += left_size + 1;
        }
        current = current->parent_;
      }
      return ans;
    }
  };

  // CONSTRUCTOR DESTRUCTOR

  // private:
  Node *root_ = nullptr;
  FastRand rand_;
  TREAP_CONSTEXPR_AFTER_CXX17
  BasicTreap(Node *root) : root_(root) {}

public:
  TREAP_CONSTEXPR_AFTER_CXX17
  BasicTreap() = default;

  TREAP_CONSTEXPR_AFTER_CXX17
  void swap(BasicTreap &other) noexcept { std::swap(root_, other.root_); }

  TREAP_CONSTEXPR_AFTER_CXX17
  BasicTreap(BasicTreap &&other) noexcept { std::swap(root_, other.root_); }

  TREAP_CONSTEXPR_AFTER_CXX17
  BasicTreap &operator=(BasicTreap &&other) noexcept {
    std::swap(root_, other.root_);
    return *this;
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  BasicTreap(const BasicTreap &other) {
    for (auto &q : other) {
      BasicTreap t;
      t.emplace_root(q);
      *this << t;
    }
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  BasicTreap &operator=(const BasicTreap &other) {
    if (this != &other) {
      clear();
      for (auto &q : other) {
        BasicTreap t;
        t.emplace_root(q);
        *this << t;
      }
    }
    return *this;
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  ~BasicTreap() { clear(); }

  // ITERATOR

protected:
  struct Iter {
    // private:
    using difference_type = std::ptrdiff_t;
    using value_type = const T;
    using pointer = const T *;
    using reference = const T &;
    using iterator_category = std::random_access_iterator_tag;
    using param = T;
    Node *current_ = nullptr;
    bool is_end_ = false;

  public:
    // TREAP_CONSTEXPR_AFTER_CXX17
    // auto tuple()const{
    //     return make_tuple(current_, is_end_);
    // }
    // TREAP_CONSTEXPR_AFTER_CXX17
    // static Iter make_iter(Node *current = nullptr, bool is_end = false) {
    //   Iter res;
    //   res.current_ = current;
    //   res.is_end_ = is_end;
    //   return res;
    // }

    // TODO: const Node*
    TREAP_CONSTEXPR_AFTER_CXX17
    operator Node *() { return current_; }

    // ITER DEREFERENCE

    TREAP_CONSTEXPR_AFTER_CXX17
    const T &operator*() const { return current_->value_; }
    TREAP_CONSTEXPR_AFTER_CXX17
    const T *operator->() const { return &**this; }
    TREAP_CONSTEXPR_AFTER_CXX17
    const T &operator[](std::ptrdiff_t offset) const {
      return *(*this + offset);
    }

    // ITER ARITHMETICS

    TREAP_CONSTEXPR_AFTER_CXX17
    Iter &operator+=(std::ptrdiff_t offset) {
      if (not current_) {
        return *this;
      }
      if (is_end_) {
        offset += 1;
        is_end_ = 0;
      }
      auto tmp = Node::add(current_, offset);
      if (not tmp.second) {
        current_ = tmp.first;
      } else {
        current_ = tmp.first;
        auto child = current_->get_child(1);
        auto child_size = child ? child->size_ : 0;
        current_ = Node::add(current_, child_size).first;
        is_end_ = 1;
      }
      return *this;
    }
    TREAP_CONSTEXPR_AFTER_CXX17
    Iter &operator-=(std::ptrdiff_t add) { return this[0] += -add; }
    TREAP_CONSTEXPR_AFTER_CXX17
    Iter &operator++() { return this[0] += 1; }
    TREAP_CONSTEXPR_AFTER_CXX17
    Iter &operator--() { return this[0] += -1; }
    TREAP_CONSTEXPR_AFTER_CXX17
    Iter &operator++(int) {
      auto tmp = *this;
      this[0]++;
      return tmp;
    }
    TREAP_CONSTEXPR_AFTER_CXX17
    Iter &operator--(int) {
      auto tmp = *this;
      this[0]--;
      return tmp;
    }
    TREAP_CONSTEXPR_AFTER_CXX17
    std::ptrdiff_t root_offset() const {
      return Node::root_offset(current_) + is_end_;
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend Iter operator+(Iter left, std::ptrdiff_t right) {
      return left += right;
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend Iter operator+(std::ptrdiff_t left, Iter right) {
      return right += left;
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend Iter operator-(Iter left, std::ptrdiff_t right) {
      return left -= right;
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend std::ptrdiff_t operator-(Iter left, Iter right) {
      return left.root_offset() - right.root_offset();
    }

    // ITER COMPARE

    TREAP_CONSTEXPR_AFTER_CXX17
    friend bool operator==(Iter left, Iter right) {
      return left.current_ == right.current_ and left.is_end_ == right.is_end_;
    }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend bool operator!=(Iter left, Iter right) { return not(left == right); }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend bool operator<(Iter left, Iter right) { return left - right < 0; }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend bool operator>(Iter left, Iter right) { return left - right > 0; }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend bool operator<=(Iter left, Iter right) { return left - right <= 0; }

    TREAP_CONSTEXPR_AFTER_CXX17
    friend bool operator>=(Iter left, Iter right) { return left - right >= 0; }
  };

  // BEGIN END

public:
  TREAP_CONSTEXPR_AFTER_CXX17
  Node *root() const { return root_; }

private:
  template <size_t iter_index>
  TREAP_CONSTEXPR_AFTER_CXX17 Iter get_iter() const {
    auto it = Iter{root()};
    if (not empty()) {
      Node::make(root_);
      auto child = root_->get_child(iter_index);
      if TREAP_CONSTEXPR_AFTER_CXX17 (iter_index) {
        it += child ? child->size_ : 0;
      } else {
        it -= child ? child->size_ : 0;
      }
      it += iter_index;
    }
    return it;
  }

public:
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter cbegin() const { return get_iter<0>(); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter cend() const { return get_iter<1>(); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter begin() const { return get_iter<0>(); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter end() const { return get_iter<1>(); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter crbegin() const { return std::reverse_iterator<Iter>(get_iter<1>()); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter crend() const { return std::reverse_iterator<Iter>(get_iter<0>()); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter rbegin() const { return std::reverse_iterator<Iter>(get_iter<1>()); }
  TREAP_CONSTEXPR_AFTER_CXX17
  Iter rend() const { return std::reverse_iterator<Iter>(get_iter<0>()); }
  const T &back() const { return *rbegin(); }
  const T &front() const { return *begin(); }

  // SIZE

  TREAP_CONSTEXPR_AFTER_CXX17
  std::size_t size() const {
    Node::make(root_);
    return root_ ? root_->size_ : 0;
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  bool empty() const { return not root_; }

  // CHANGE SIZE

  template <typename... CON_ARGS>
  TREAP_CONSTEXPR_AFTER_CXX17 void emplace_root(CON_ARGS &&...args) {
    clear();
    root_ = new Node(rand_, std::forward<CON_ARGS>(args)...);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  void clear() {
    Node::del(root_);
    root_ = nullptr;
  }

  // IO

  template <typename OSTREAM>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      not std::is_base_of_v<BasicTreap, OSTREAM>, OSTREAM> &
  operator<<(OSTREAM &out, BasicTreap &treap) {
    Node::print(treap.root_, out);
    return out;
  }

  // ALIASES

  using value_type = T;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using iterator = Iter;
  using const_iterator = Iter;
  using reverse_iterator = std::reverse_iterator<Iter>;
  using const_reverse_iterator = const std::reverse_iterator<Iter>;

  // SPLIT AND MERGE

  template <typename UNARY_PREDICATE, typename TREAP>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      std::is_base_of_v<BasicTreap, TREAP> and
          std::is_invocable_v<UNARY_PREDICATE, BasicTreap::Node *>,
      TREAP>
  operator|(TREAP &left, UNARY_PREDICATE &&is_right) {
    auto pair = Node::template split<1>(
        left.root_, std::forward<UNARY_PREDICATE>(is_right));
    left.root_ = pair[0];
    return {pair[1]};
  }

  template <typename UNARY_PREDICATE, typename TREAP>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      std::is_base_of_v<BasicTreap, TREAP> and
          std::is_invocable_v<UNARY_PREDICATE, BasicTreap::Node *>,
      TREAP>
  operator|(UNARY_PREDICATE &&is_left, TREAP &right) {
    auto pair = Node::template split<0>(right.root_,
                                        std::forward<UNARY_PREDICATE>(is_left));
    right.root_ = pair[1];
    return {pair[0]};
  }

  template <typename TREAP>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      std::is_base_of_v<BasicTreap, TREAP>, TREAP> &
  operator<<(TREAP &left, TREAP &right) {
    left.root_ = Node::merge({left.root_, right.root_}, true);
    right.root_ = nullptr;
    return left;
  }

  template <typename TREAP>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      std::is_base_of_v<BasicTreap, TREAP>, TREAP> &
  operator<<(TREAP &left, TREAP &&right) {
    left.root_ = Node::merge({left.root_, right.root_}, true);
    right.root_ = nullptr;
    return left;
  }

  template <typename TREAP>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      std::is_base_of_v<BasicTreap, TREAP>, TREAP> &
  operator>>(TREAP &left, TREAP &right) {
    right.root_ = Node::merge({left.root_, right.root_}, true);
    left.root_ = nullptr;
    return right;
  }

  template <typename TREAP>
  TREAP_CONSTEXPR_AFTER_CXX17 friend std::enable_if_t<
      std::is_base_of_v<BasicTreap, TREAP>, TREAP> &
  operator>>(TREAP &&left, TREAP &right) {
    right.root_ = Node::merge({left.root_, right.root_}, true);
    left.root_ = nullptr;
    return right;
  }

  // TREAP COMPARE

  TREAP_CONSTEXPR_AFTER_CXX17
  friend bool operator==(const BasicTreap &left, const BasicTreap &right) {
    return left.size() == right.size() and
           std::equal(left.begin(), left.end(), right.begin());
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  friend bool operator<(const BasicTreap &left, const BasicTreap &right) {
    return std::lexicographical_compare(left.begin(), left.end(), right.begin(),
                                        right.end());
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  friend bool operator>(const BasicTreap &left, const BasicTreap &right) {
    return right < left;
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  friend bool operator!=(const BasicTreap &left, const BasicTreap &right) {
    return not(left == right);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  friend bool operator<=(const BasicTreap &left, const BasicTreap &right) {
    return not(left > right);
  }

  TREAP_CONSTEXPR_AFTER_CXX17
  friend bool operator>=(const BasicTreap &left, const BasicTreap &right) {
    return not(left < right);
  }
};
