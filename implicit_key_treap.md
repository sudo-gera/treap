# `ImplicitTreap<typename T>:BasicTreap<T>`
An indexed sequence container.

ImplicitTreap mimics interface of `std::deque<T>`
## operations:
### (all BasicTreap operations are inherited)
### (all operations from `std::deque<T>`)
Almost all operations are `O(log(n))` instead of `O(1)` in deque `(n==size())`.
### n | treap
Cut n elemens from left.
return treap with cut elements. Other elements are left in given treap.
### treap | n
Cut n elemens from right.
return treap with cut elements. Other elements are left in given treap.
