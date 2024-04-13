# Small C++ Wrapper Classes

The classes in this library make it easier to enforce envariants in larger classes.
None of them serve a business need directly, but they all make it easier to write classes that do.

## `propagate_const`

Ensures that a const pointer also points to a const object.

This type has been suggested as an addition to C++'s standard library. (Proposal [N4388](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4388.html))

## `not_null`

Ensures that a pointer can never be null.