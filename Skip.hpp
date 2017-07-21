#pragma once

#include "IteratorAdapter.hpp"

#include <algorithm>

namespace Linqpp
{
    namespace Detail
    {
        template <class Iterator>
        auto InternalSkip(Iterator first, size_t n, Iterator last, std::random_access_iterator_tag)
        {
           return std::min(first + n, last);
        }

        template <class Iterator, class Category>
        auto InternalSkip(Iterator first, size_t n, Iterator last, Category)
        {
			for (size_t i = 0; i < n && first != last; ++i, ++first);
			
			return first;
        }
    }

    template <class Iterator> 
    auto Skip(Iterator first, size_t n, Iterator last)
    {
       return Detail::InternalSkip(first, n, last, typename std::iterator_traits<Iterator>::iterator_category());
    }
}
