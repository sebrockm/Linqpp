#pragma once

#include <algorithm>

namespace Linqpp
{
    template <class InputIterator, class Predicate>
    auto SkipWhile(InputIterator first, InputIterator last, Predicate predicate,
        decltype(predicate(std::declval<typename std::iterator_traits<InputIterator>::value_type>()))* = nullptr)
    {
        for (; first != last && predicate(*first); ++first);
        return first;
    }

    template <class InputIterator, class Predicate>
    auto SkipWhile(InputIterator first, InputIterator last, Predicate predicate,
        decltype(predicate(std::declval<typename std::iterator_traits<InputIterator>::value_type>(), 0))* = nullptr)
    {
        for (size_t i = 0; first != last && predicate(*first, i); ++i, ++first);
        return first;
    }
}
