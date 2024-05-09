# Small C++ Wrapper Classes

The classes in this library make it easier to enforce invariants in larger classes.
None of them serve a business need directly, but they all make it easier to write classes that do.

The classes will be covered in my upcoming talk at C++ on Sea 2024, [The Surprising Power of Small Wrapper Classes
](https://cpponsea.uk/2024/sessions/the-surprising-power-of-small-wrapper-classes.html).

## `propagate_const<PointerType>`

Ensures that a const pointer also points to a const object.

This type has been suggested as an addition to C++'s standard library. (Proposal [N4388](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4388.html))

## `not_null<PointerType>`

Ensures that a pointer can never be null.

## `lazy_init<T>`

Ensures that the wrapped data is initialized exactly once, in an exception safe manner.
