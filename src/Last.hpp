#pragma once

#include <iterator>
#include <algorithm>

namespace Linqpp
{
    template <class BidirectionalIterator, class Predicate>
    decltype(auto) Last(BidirectionalIterator first, BidirectionalIterator last, Predicate predicate, std::bidirectional_iterator_tag,
            typename std::iterator_traits<BidirectionalIterator>::value_type* pDefault = nullptr)
    {
        while (first != last)
        {
            --last;
            if (predicate(*last))
                return *last;
        }

        return pDefault ? *pDefault : *first;
    }

    template <class ForwardIterator, class Predicate>
    decltype(auto) Last(ForwardIterator first, ForwardIterator last, Predicate predicate, std::forward_iterator_tag,
            typename std::iterator_traits<ForwardIterator>::value_type* pDefault = nullptr)
    {
        auto found = std::find_if(first, last, predicate);

        if (found == last)
            return pDefault ? *pDefault : *first;

        while (true)
        {
            auto before = found;
            found = std::find_if(++found, last, predicate);
            if (found == last)
                return *before;
        }

        return pDefault ? *pDefault : *first;
    }

    template <class InputIterator, class Predicate>
    auto Last(InputIterator first, InputIterator last, Predicate predicate, std::input_iterator_tag,
            typename std::iterator_traits<InputIterator>::value_type* pDefault = nullptr)
    {
        auto found = std::find_if(first, last, predicate);

        if (found == last)
            return pDefault ? *pDefault : *first;

        while (true)
        {
            auto before = *found;
            found = std::find_if(++found, last, predicate);
            if (found == last)
                return before;
        }

        return pDefault ? *pDefault : *first;
    }
}
