#pragma once

namespace rfx
{
    template<typename Range, typename Item>
    bool contains(const Range& range, const Item& item)
    {
        return std::find(cbegin(range), cend(range), item) != cend(range);
    }

} // namespace rfx