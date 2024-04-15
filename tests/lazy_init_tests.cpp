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
;

struct not_default_constructible
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

struct equality_comparable final
{
	int value;
	bool operator==(equality_comparable const&) const = default;
};

TEST(LazyInit, ArrowOperator)
{
	for (auto i : std::ranges::iota_view(5, 10))
	{
		sw::lazy_init<equality_comparable> li{
			[&] {
				return equality_comparable{i};
			} };
		sw::lazy_init<equality_comparable> const& const_ref = li;
		EXPECT_EQ(li->value, i);
		EXPECT_EQ(const_ref->value, i);
	}
}

TEST(LazyInit, CompareEquality)
{
	sw::lazy_init<equality_comparable> li0{
		[] {
			return equality_comparable{0};
		} };
	
	sw::lazy_init<equality_comparable> li1{
		[] {
			return equality_comparable{1};
		} };
	sw::lazy_init<equality_comparable> const& const_ref = li0;
	EXPECT_TRUE(li0 == const_ref);
	EXPECT_TRUE(li0 != li1);
	EXPECT_TRUE(const_ref != li1);
	EXPECT_FALSE(li0 == li1);
}

struct not_equality_comparable final
{
	int value;
	bool operator==(equality_comparable const&) const = delete;
};

// This test ensures that we don't accidentally require the value type to be equality comparable,
// unless we actually try to compare two values.
TEST(LazyInit, UncomparableClassCanBeConstructed)
{
	sw::lazy_init<not_equality_comparable> li{
		[] {
			return not_equality_comparable{0};
		} };
	EXPECT_EQ((*li).value, 0);
}



// TODO:
// Implement operators
//  Spaceship (conditionally)
// Ensure thread safety.
// Null-check function pointers.
// Copy-only function types