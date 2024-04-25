#include "../sw/propagate_const.h"

#include "gtest_unwarn.h"

#include <type_traits>

template <typename T>
concept propagate_const_specialization =
    std::is_move_constructible_v<T> ==
    std::is_move_constructible_v<typename T::element_type>;
static_assert(propagate_const_specialization<sw::propagate_const<int *>>);

struct call_tester final {
  bool non_const_call = false;
  mutable bool const_call = false;

  void call() { non_const_call = true; }

  void call() const { const_call = true; }
};

TEST(PropagateConst, ArrowPropagatesNonConstRawPointer) {
  call_tester ct;
  sw::propagate_const<call_tester *> pc{&ct};
  pc->call();
  EXPECT_TRUE(ct.non_const_call);
}

TEST(PropagateConst, ArrowPropagatesConstRawPointer) {
  call_tester ct;
  sw::propagate_const<call_tester *> const pc{&ct};
  pc->call();
  EXPECT_TRUE(ct.const_call);
}

TEST(PropagateConst, ArrowPropagatesNonConstUniquePointer) {
  auto underlying = std::make_unique<call_tester>();
  call_tester &ct{*underlying};
  sw::propagate_const<std::unique_ptr<call_tester>> pc{std::move(underlying)};
  pc->call();
  EXPECT_TRUE(ct.non_const_call);
}

TEST(PropagateConst, ArrowPropagatesConstUniquePointer) {
  auto underlying = std::make_unique<call_tester>();
  call_tester &ct{*underlying};
  sw::propagate_const<std::unique_ptr<call_tester>> const pc{
      std::move(underlying)};
  pc->call();
  EXPECT_TRUE(ct.const_call);
}

TEST(PropagateConst, ArrowPropagatesNonConstSharedPointer) {
  auto underlying = std::make_shared<call_tester>();
  call_tester &ct{*underlying};
  sw::propagate_const<std::shared_ptr<call_tester>> pc{std::move(underlying)};
  pc->call();
  EXPECT_TRUE(ct.non_const_call);
}

TEST(PropagateConst, ArrowPropagatesConstSharedPointer) {
  auto underlying = std::make_shared<call_tester>();
  call_tester &ct{*underlying};
  sw::propagate_const<std::shared_ptr<call_tester>> const pc{
      std::move(underlying)};
  pc->call();
  EXPECT_TRUE(ct.const_call);
}