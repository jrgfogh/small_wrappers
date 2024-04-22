#include "../sw/propagate_const.h"

#include "gtest_unwarn.h"

#include <type_traits>

template<typename T>
concept propagate_const_specialization =
    std::is_move_constructible_v<T> == std::is_move_constructible_v<typename T::element_type>
;
static_assert(propagate_const_specialization<propagate_const<int*>>);

TEST(PropagateConst, Exists) {
}