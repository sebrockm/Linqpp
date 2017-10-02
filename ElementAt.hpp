#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Iterator>
    decltype(auto) ElementAt(Iterator first, int64_t i, std::random_access_iterator_tag)
    {
        return first[i];
    }

    template <class Iterator>
    decltype(auto) ElementAt(Iterator first, int64_t i, std::input_iterator_tag)
    {
        for (; i > 0; --i, ++first);
        
        return *first;
    }

    template <class Iterator>
    typename std::iterator_traits<Iterator>::value_type ElementAtOrDefault(Iterator first, Iterator last, int64_t i, std::random_access_iterator_tag)
    {
        if (i >= 0 && i < last - first) 
            return first[i];
        return { };
    }

    template <class Iterator>
    typename std::iterator_traits<Iterator>::value_type ElementAtOrDefault(Iterator first, Iterator last, int64_t i, std::input_iterator_tag)
    {
        if (i < 0)
            return { };

        for (; i != 0 && first != last; --i, ++first);
        
        if (first != last)
            return *first;
        return { };
    }
}
