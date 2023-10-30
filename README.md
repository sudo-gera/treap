# Treap
universal extendable constexpr treap implementation

## Docs for each file:
[`./basic_treap.hpp`](./basic_treap.md)

[`./implicit_key_treap.hpp`](./implicit_key_treap.md)

[`./explicit_key_treap.hpp`](./explicit_key_treap.md)

[`./add_and_sum_treap.hpp`](./add_and_sum_treap.md)

[`./many_operations_treap.hpp`](./many_operations_treap.md)

## Usage:
`#include` is all you need. This is header-only library.

## Run tests:
all tests are in `test_*.cpp` files. You can compile them with g++ or clang++ with any standart >= 11 and with any sanitizers/optimizations if you want. Using standart >= 20 would also include constexpr-evaluated tests.