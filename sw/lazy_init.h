#pragma once

#include <mutex>
#include <utility>
#include <memory>
#include <concepts>
#include <functional>

namespace sw
{
	template <typename ValueType, typename InitType = std::move_only_function<typename ValueType()>>
		requires std::invocable<InitType>
	class lazy_init final
	{
		mutable std::unique_ptr<ValueType> data_;
		mutable InitType init_;

		void ensure_initialized() const
		{
			if (!data_)
			{
				data_ = std::make_unique<ValueType>(init_());
			}
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

		auto operator->() -> ValueType*
		{
			ensure_initialized();
			return data_.get();
		}

		auto operator->() const -> ValueType const*
		{
			ensure_initialized();
			return data_.get();
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