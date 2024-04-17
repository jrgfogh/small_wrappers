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


struct not_assignable
{
	auto const &operator=(not_assignable const&) = delete;
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
	*li;
}

TEST(LazyInit, DoesNotRequireAssignableData)
{
	sw::lazy_init<not_assignable> li{
		[] {
			return not_assignable{};
		}};
	*li;
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

struct comparable final
{
	int value;
	bool operator==(comparable const&) const = default;
	std::strong_ordering operator<=>(comparable const&) const = default;
};

TEST(LazyInit, ArrowOperator)
{
	for (auto i : std::ranges::iota_view(5, 10))
	{
		sw::lazy_init<comparable> li{
			[&] {
				return comparable{i};
			} };
		sw::lazy_init<comparable> const& const_ref = li;
		EXPECT_EQ(li->value, i);
		EXPECT_EQ(const_ref->value, i);
	}
}

TEST(LazyInit, CompareEquality)
{
	sw::lazy_init<comparable> li0{
		[] {
			return comparable{0};
		} };
	
	sw::lazy_init<comparable> li1{
		[] {
			return comparable{1};
		} };
	sw::lazy_init<comparable> const& const_ref = li0;
	EXPECT_TRUE(li0 == const_ref);
	EXPECT_TRUE(li0 != li1);
	EXPECT_TRUE(const_ref != li1);
	EXPECT_FALSE(li0 == li1);
}

TEST(LazyInit, CompareSpaceship)
{
	sw::lazy_init<comparable> li0{
		[] {
			return comparable{0};
		} };
	
	sw::lazy_init<comparable> li1{
		[] {
			return comparable{1};
		} };
	sw::lazy_init<comparable> const& const_ref = li0;
	EXPECT_EQ(li0 <=> const_ref, std::strong_ordering::equal);
	EXPECT_LT(li0, li1);
	EXPECT_LT(const_ref, li1);
	EXPECT_NE(li0 <=> li1, std::strong_ordering::equal);
}

struct not_comparable final
{
	int value;
	bool operator==(comparable const&) const = delete;
};

// This test ensures that we don't accidentally require the value type to be equality comparable,
// unless we actually try to compare two values.
TEST(LazyInit, UncomparableClassCanBeConstructed)
{
	sw::lazy_init<not_comparable> li{
		[] {
			return not_comparable{0};
		} };
	EXPECT_EQ((*li).value, 0);
}

struct copy_only_functor
{
	explicit copy_only_functor() = default;
	copy_only_functor(copy_only_functor &&) = delete;
	copy_only_functor(copy_only_functor const&) = default;
	int operator()() { return 5; }
};

auto x = std::is_move_constructible_v<copy_only_functor>;

TEST(LazyInit, CopyOnlyFunctorIsValidInitType)
{
	copy_only_functor functor;
	sw::lazy_init<int, copy_only_functor> li{functor};
	EXPECT_EQ(*li, 5);
}

//TEST(LazyInit, NullCheckInitFunction)
//{
//	EXPECT_THROW((sw::lazy_init<int>{nullptr}), std::exception);
//	std::function<int()> init = nullptr;
//	EXPECT_THROW((sw::lazy_init<int, std::function<int()>>{init}), std::exception);
//}

// TODO:
// Null-check function pointers.