#pragma once

#include <utility>
#include <memory>
#include <concepts>
#include <functional>
#include <optional>

namespace sw
{
	template <typename ValueType, typename InitType = std::move_only_function<typename ValueType()>>
		requires std::invocable<InitType>
	class lazy_init final
	{
		mutable std::optional<ValueType> data_;
		mutable InitType init_;

		void ensure_initialized() const
		{
			if (!data_)
				data_.emplace(init_());
		}
	public:
		using value_type = ValueType;
		using init_type = InitType;

		explicit lazy_init(InitType &&init) :
			init_{std::move(init)}
		{
		}

		explicit lazy_init(InitType const &init) :
			init_{init}
		{
		}

		auto operator*() -> ValueType&
		{
			ensure_initialized();
			return *data_;
		}

		auto operator*() const -> ValueType const &
		{
			ensure_initialized();
			return *data_;
		}

		auto operator->()
		{
			ensure_initialized();
			return data_;
		}

		auto operator->() const
		{
			ensure_initialized();
			return data_;
		}

		bool operator==(lazy_init const& that) const
		{
			return operator*() == *that;
		}

		auto operator<=>(lazy_init const& that) const -> std::strong_ordering
		{
			return operator*() <=> *that;
		}
	};
}