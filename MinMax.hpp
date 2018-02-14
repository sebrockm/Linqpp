#pragma once

#include <algorithm>
#include <iterator>

namespace Linqpp
{
    namespace Detail
    {
        template <class ForwardIterator>
        decltype(auto) InternalMax(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag) { return *std::max_element(first, last); }

        template <class ForwardIterator>
        decltype(auto) InternalMin(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag) { return *std::min_element(first, last); }

        template <class InputIterator>
        auto InternalMax(InputIterator first, InputIterator last, std::input_iterator_tag)
        {
            if (first == last)
                return *first;

            auto maxValue = *first;
            ++first;
            for (;first != last; ++first)
            {
                auto temp = *first;
                if (temp > maxValue)
                    maxValue = std::move(temp);
            }
            return maxValue;
        }

        template <class InputIterator>
        auto InternalMin(InputIterator first, InputIterator last, std::input_iterator_tag)
        {
            if (first == last)
                return *first;

            auto minValue = *first;
            ++first;
            for (;first != last; ++first)
            {
                auto temp = *first;
                if (temp < minValue)
                    minValue = std::move(temp);
            }
            return minValue;
        }
    }

    template <class InputIterator>
    decltype(auto) Max(InputIterator first, InputIterator last)
    {
        return Detail::InternalMax(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
    }

    template <class InputIterator>
    decltype(auto) Min(InputIterator first, InputIterator last)
    {
        return Detail::InternalMin(first, last, typename std::iterator_traits<InputIterator>::iterator_category());
    }
}
