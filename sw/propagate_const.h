#pragma once

#include <utility>

template <typename T>
class propagate_const final
{
    T t_;
public:
    using element_type = int;

    constexpr propagate_const() = default;
    constexpr propagate_const( propagate_const&& p ) = default;
    propagate_const( const propagate_const& ) = delete;

    constexpr propagate_const& operator=(propagate_const&&) = default;
    template< class U >
    constexpr propagate_const& operator=(U&& u)
    {
        t_ = std::forward<U>(u);
    }
    propagate_const& operator=( const propagate_const& ) = delete;
};