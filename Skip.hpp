#pragma once

#include "IteratorAdapter.hpp"

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
}
