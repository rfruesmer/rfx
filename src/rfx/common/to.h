#pragma once

namespace rfx::details {
    template<template<typename...> class>
    struct to_container {};

    template<std::ranges::viewable_range R, template<typename...> class ContainerType>
    constexpr auto operator|(R&& range, to_container<ContainerType> const& a) {
        using ValueType = std::ranges::range_value_t<decltype(range)>;
        ContainerType<ValueType> result{};
        if constexpr(std::ranges::sized_range<decltype(range)>) {
            result.reserve(std::ranges::size(range));
        }
        std::ranges::copy(range, std::back_inserter(result));

        return result;
    }
}

namespace rfx
{
    template<template<typename...> class ContainerType>
    constexpr auto to() {
        return details::to_container<ContainerType>();
    }
}
