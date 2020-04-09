#pragma once

#include <algorithm>

namespace Linqpp
{
    namespace Detail
    {
        template <class RandomIterator>
        auto InternalSkip(RandomIterator first, size_t n, RandomIterator last, std::random_access_iterator_tag)
        {
            using diff_t = typename std::iterator_traits<RandomIterator>::difference_type;
            return first + std::min((diff_t) n, last - first);
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
