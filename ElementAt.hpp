#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Iterator>
    decltype(auto) ElementAt(Iterator first, size_t i, std::random_access_iterator_tag)
    {
        return first[i];
    }

    template <class Iterator>
    decltype(auto) ElementAt(Iterator first, size_t i, std::input_iterator_tag)
    {
        for (; i != 0; --i, ++first);
        
        return *first;
    }

    template <class Iterator>
    decltype(auto) ElementAtOrDefault(Iterator first, Iterator last, size_t i, std::random_access_iterator_tag)
    {
        return first + i < last ? first[i] : typename std::iterator_traits<Iterator>::value_type();
    }

    template <class Iterator>
    decltype(auto) ElementAtOrDefault(Iterator first, Iterator last, size_t i, std::input_iterator_tag)
    {
        for (; i != 0 && first != last; --i, ++first);
        
        return first != last ? *first : typename std::iterator_traits<Iterator>::value_type();
    }
}
