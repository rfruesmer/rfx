#pragma once

namespace rfx
{

template<typename Range, typename Function>
Function for_each(Range& range, Function fn)
{
    return std::for_each(begin(range), end(range), fn);
}

template<typename Range, typename OutputIt, typename Function>
OutputIt transform(Range& range, OutputIt out, Function fn)
{
    return std::transform(begin(range), end(range), out, fn);
}

template<typename Range, typename Item>
bool contains(const Range& range, const Item& item)
{
    return std::find(cbegin(range), cend(range), item) != cend(range);
}

} // namespace rfx
