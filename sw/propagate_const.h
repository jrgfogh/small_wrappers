#pragma once

#include <utility>
#include <type_traits>
#include <memory>

namespace sw
{
    template <typename T>
    class propagate_const final
    {
        T t_;
    public:
        using element_type = typename std::pointer_traits<T>::element_type;

        constexpr propagate_const() = default;
        constexpr propagate_const(propagate_const&&) = default;
        template <class U>
        constexpr explicit(!std::is_convertible<U, T>::value) propagate_const(U &&u) :
            t_{std::forward<U>(u)}
        {}
        propagate_const(const propagate_const&) = delete;

        constexpr propagate_const& operator=(propagate_const&&) = default;
        template <class U>
        constexpr propagate_const& operator=(U&& u)
        {
            t_ = std::forward<U>(u);
            return *this;
        }
        propagate_const& operator=(const propagate_const&) = delete;

        constexpr element_type* operator->()
        {
            return std::to_address(t_);
        }

        constexpr element_type const* operator->() const
        {
            return std::to_address(t_);
        }
    };
}