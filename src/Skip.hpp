#pragma once

#include "From.hpp"
#include "iterator/SkipWhileIterator.hpp"

#include <algorithm>

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last);

    namespace Detail
    {
        template <class RandomIterator>
        auto InternalGetEnumerableFromSkip(RandomIterator first, RandomIterator last, size_t n, std::random_access_iterator_tag)
        {
            using diff_t = typename std::iterator_traits<RandomIterator>::difference_type;
            return From(first + std::min((diff_t) n, last - first), last);
        }

        template <class InputIterator>
        auto InternalGetEnumerableFromSkip(InputIterator first, InputIterator last, size_t n, std::input_iterator_tag)
        {
            return GetEnumerableFromSkipWhile(first, last, [=](auto, size_t i) { return i < n; });
        }
    }

    template <class InputIterator>
    auto GetEnumerableFromSkip(InputIterator first, InputIterator last, size_t n)
    {
       return Detail::InternalGetEnumerableFromSkip(first, last, n, typename std::iterator_traits<InputIterator>::iterator_category());
    }
}
