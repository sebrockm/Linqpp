#pragma once

#include <iterator>

namespace Linqpp
{
    template <class RandomIterator>
    decltype(auto) ElementAt(RandomIterator first, int64_t i, std::random_access_iterator_tag)
    {
        return first[i];
    }

    template <class InputIterator>
    decltype(auto) ElementAt(InputIterator first, int64_t i, std::input_iterator_tag)
    {
        for (; i > 0; --i, ++first);
        
        return *first;
    }

    template <class RandomIterator>
    typename std::iterator_traits<RandomIterator>::value_type ElementAtOrDefault(RandomIterator first, RandomIterator last, int64_t i, std::random_access_iterator_tag)
    {
        if (i >= 0 && i < last - first) 
            return first[i];
        return { };
    }

    template <class InputIterator>
    typename std::iterator_traits<InputIterator>::value_type ElementAtOrDefault(InputIterator first, InputIterator last, int64_t i, std::input_iterator_tag)
    {
        if (i < 0)
            return { };

        for (; i != 0 && first != last; --i, ++first);
        
        if (first != last)
            return *first;
        return { };
    }
}
