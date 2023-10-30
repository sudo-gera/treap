# `BasicTreap<typename T>`
Treap with only basic operations.
## `BasicTreap<T>::Node`
`Node` is a subclass for BasicTreap used to store treap nodes. It was made public to allow more used-defined features.
Node has several members:

    std::size_t priority_; // larger priority - closer to root
    std::array<Node *, 2> children_;
    Node *parent_;
    std::size_t size_; // size of subtree, 1 if no children
    std::size_t height_; // height of subtree, 1 if no children
    T value_; // actual value, all other values needed by user-defined features should be members of T.

All members are allowed for changing by `T::make`, `T::update`, `treap::root_node()`.
## Requirements for T:
### `static void make(Node*)`
Should make all promises stored in given node and reassign them to children. After calling, given node should be free from any promises.
### `static void update(Node*)`
Called after each change of `children` member of given node. Should recalculate all values. Members `size_` and `height_` are precalculated before the call.
### `static void to_string(Node*, ostream&)`
Called for ostream &lt;&lt; treap.
### Precalculated values and promises
All values should be calculated as if there were no promises.
#### Correct example:

    size_ == 1
    value_.value == 4
    value_.sum == 4
    value_.to_add == 5
#### Incorrect example:

    size_ == 1
    value_.value == 4
    value_.sum == 9
    value_.to_add == 5
## Avaliable methods:
### (default constructor)
### (move constructor/opetrator)
Changes internal pointers. No values are moved.
### (copy constructor/operator)
Requires T to be copyable.
### size()
### empty()
### clear()
### operators for lexicographical compare
### `treapA << treapB`
Concat (aka merge) treapA and treapB (in this order). Write result to treapA. TreapB would be empty.
### `treap1 >> treap2`
Concat (aka merge) treapA and treapB (in this order). Write result to treapB. TreapA would be empty.
### `bool IsLeft(Node*) | treap`
Cut several nodes from left(aka split treap).
`IsLeft` answers `true`, if node should be cut and `false` if should be left in given treap.
Returns treap containing nodes from left half (cut nodes), nodes from right half are left in given treap.
### `bool treap | IsRight(Node*)`
Cut several nodes from right(aka split treap).
`IsRight` answers `true`, if node should be cut and `false` if should be left in given treap.
Returns treap containing nodes from right half (cut nodes), nodes from left half are left in given treap.
### `operator|` call order (for both cases):
UnaryPredicate of operator| when called on nodes, answers if border should be to the left or right from this node. First node to call is root, each next node is a child of previous node.
### `ostream << treap`
Writes multiline structire of treap to ostream. Would call `T::to_string` for each node. Example:

     ┏━► 21
    ┏╋► 20
    ┃┗┳► 19
    ┃ ┗┳► 18
    ┃  ┗┳► 17
    ┃   ┗━► 16
    ╋► 15
    ┃┏━► 14
    ┗╋► 13
     ┃┏━► 12
     ┗┻► 11

### emplace_root(auto...args)
1. `treap.clear();`
2. Construct value for root node in-place.

#### Node* root()
Return root node. You can edit it, for example, for assigning a promise. Editing invalidates all iterators.

## iterators
Only const iterators are supported.

Iterators have two members:

    Node* current_;
    bool is_end_;
If treap is not empty, `is_end_` can be true if and only if `current_` is last node in treap.
If treap is empty, `is_end_` is always true and `current_` is `nullptr`.
It is important to know because it helps to understand when iterators are invalidated and when are not.

Iterators meet all requirements of bidirectional iterators.
Iterators meet almost all requirements of random access iterators, except that they work `O(log(n))` on all operations that are not checked by bidirectional requirements. `(n == size())`

### treap::iterator.operator Node*
Convert not equal to `end()` iterator to Node*

### treap::iterator::make_iter(Node*, bool)
If bool is false, creates iterator to given node.
If bool is true and Node* points to last element, creates end() iterator.
Otherwise behavior is undefined.


