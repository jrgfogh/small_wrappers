#include "../sw/lazy_init.h"

#include "gtest_unwarn.h"

#include <type_traits>
#include <utility>
#include <memory>
#include <string>
#include <ranges>

template<typename T>
concept lazy_init_specialization =
	std::is_constructible_v<T, typename T::init_type> &&
	std::is_same_v<decltype(*std::declval<T>()), typename T::value_type&> &&
	std::is_same_v<decltype(*std::declval<T const>()), typename T::value_type const&>
	// I'm not sure if this is a bug or a feature, but at least it's documented here:
	//!std::is_move_assignable_v<T> &&
	//!std::is_move_constructible_v<T> &&
	//!std::is_copy_assignable_v<T> &&
	//!std::is_copy_constructible_v<T>
;

struct not_default_constructible alignas(16)
{
	explicit not_default_constructible() = delete;
	explicit not_default_constructible(int) {}
};

static_assert(lazy_init_specialization<sw::lazy_init<double, int(*)()>>);
static_assert(lazy_init_specialization<sw::lazy_init<not_default_constructible, not_default_constructible(*)()>>);
static_assert(lazy_init_specialization<sw::lazy_init<std::string, std::string(*)()>>);
static_assert(lazy_init_specialization<sw::lazy_init<std::shared_ptr<int>, std::nullptr_t(*)()>>);

TEST(LazyInit, IsLazy)
{
	bool called = false;
	sw::lazy_init<int> li{
		[&] {
			called = true;
			return 5;
		}};
	EXPECT_FALSE(called);
}

TEST(LazyInit, InitExactlyOnce)
{
	int callCount = 0;
	sw::lazy_init<int> li{
		[&] {
			callCount++;
			return 5;
		}};
	*li;
	*li;
	*li;
	EXPECT_EQ(callCount, 1);
}

TEST(LazyInit, RetryOnFailure)
{
	bool called = false;
	sw::lazy_init<int> li{
		[&] {
			if (!called)
			{
				called = true;
				throw std::exception{};
			}
			return 5;
		}};
	EXPECT_THROW((*li), std::exception);
	EXPECT_EQ(*li, 5);
}

TEST(LazyInit, DoesNotRequireDefaultConstructibleData)
{
	sw::lazy_init<not_default_constructible> li{
		[] {
			return not_default_constructible{5};
		}};
}

TEST(LazyInit, StarOperator)
{
	for (auto i : std::ranges::iota_view(5, 10))
	{
		sw::lazy_init<int> li{
			[&] {
				return i;
			} };
		sw::lazy_init<int> const& const_ref = li;
		EXPECT_EQ(*li, i);
		EXPECT_EQ(*const_ref, i);
	}
}

// TODO:
// Implement operators
//  Arrow
//  Equality (conditionally)
//  Spaceship (conditionally)
// Ensure thread safety.
// Null-check function pointers.
// Copy-only function types