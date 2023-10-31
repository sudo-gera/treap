# `ImplicitTreap<typename T>:BasicTreap<T>`
An indexed sequence container that allows fast insertion and deletion at both its beginning and its end. In addition, insertion and deletion at either end of a deque never invalidates pointers or references to the rest of the elements.

ImplicitTreap mimics interface of `std::deque<T>`
## operations:
### (all BasicTreap operations are inherited)
### (all operations from `std::deque<T>`)
Almost all operations are `O(log(n))` instead of `O(1)` in deque, where `n` is `size()`.
### n | treap
Cut n elemens from left.
return treap with cut elements. Other elements are left in given treap.
### treap | n
Cut n elemens from right.
return treap with cut elements. Other elements are left in given treap.
