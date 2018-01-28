#pragma once

#include <algorithm>

namespace Linqpp
{
    namespace Detail
    {
        template <class RandomIterator>
        auto InternalSkip(RandomIterator first, size_t n, RandomIterator last, std::random_access_iterator_tag)
        {
           return std::min(first + n, last);
        }

        template <class InputIterator>
        auto InternalSkip(InputIterator first, size_t n, InputIterator last, std::input_iterator_tag)
        {
            for (size_t i = 0; i < n && first != last; ++i, ++first);
            
            return first;
        }
    }

    template <class InputIterator> 
    auto Skip(InputIterator first, size_t n, InputIterator last)
    {
       return Detail::InternalSkip(first, n, last, typename std::iterator_traits<InputIterator>::iterator_category());
    }

    template <class InputIterator, class Predicate>
    auto SkipWhile(InputIterator first, InputIterator last, Predicate&& predicate,
        decltype(std::forward<Predicate>(predicate)(std::declval<typename std::iterator_traits<InputIterator>::value_type>()))* = nullptr)
    {
        for (; first != last && std::forward<Predicate>(predicate)(*first); ++first);
        return first;
    }

    template <class InputIterator, class Predicate>
    auto SkipWhile(InputIterator first, InputIterator last, Predicate&& predicate,
        decltype(std::forward<Predicate>(predicate)(std::declval<typename std::iterator_traits<InputIterator>::value_type>(), 0))* = nullptr)
    {
        for (size_t i = 0; first != last && std::forward<Predicate>(predicate)(*first, i); ++i, ++first);
        return first;
    }
}
