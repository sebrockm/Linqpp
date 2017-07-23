#pragma once

#include <algorithm>
#include <iterator>

namespace Linqpp
{
    namespace Detail
    {
        template <class Iterator>
        decltype(auto) InternalMax(Iterator first, Iterator last, std::forward_iterator_tag) { return *std::max_element(first, last); }

        template <class Iterator>
        decltype(auto) InternalMin(Iterator first, Iterator last, std::forward_iterator_tag) { return *std::min_element(first, last); }

        template <class Iterator>
        decltype(auto) InternalMax(Iterator first, Iterator last, std::input_iterator_tag)
        {
            if (first == last)
                return *first;

            auto maxValue = *first;
            ++first;
            for (;first != last; ++first)
            {
                auto temp = *first;
                if (temp > maxValue)
                    maxValue = temp;
            }
            return maxValue;
        }

        template <class Iterator>
        decltype(auto) InternalMin(Iterator first, Iterator last, std::input_iterator_tag)
        {
            if (first == last)
                return *first;

            auto minValue = *first;
            ++first;
            for (;first != last; ++first)
            {
                auto temp = *first;
                if (temp < minValue)
                    minValue = temp;
            }
            return minValue;
        }
    }

    template <class Iterator>
    decltype(auto) Max(Iterator first, Iterator last)
    {
        return Detail::InternalMax(first, last, typename std::iterator_traits<Iterator>::iterator_category());
    }

    template <class Iterator>
    decltype(auto) Min(Iterator first, Iterator last)
    {
        return Detail::InternalMin(first, last, typename std::iterator_traits<Iterator>::iterator_category());
    }
}
