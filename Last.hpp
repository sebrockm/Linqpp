#pragma once

#include <iterator>
#include <algorithm>

namespace Linqpp
{
    template <class BidirectionalIterator, class Predicate>
    decltype(auto) Last(BidirectionalIterator first, BidirectionalIterator last, Predicate&& predicate, std::bidirectional_iterator_tag, bool& isValid)
    {
        isValid = true;
        while (first != last)
        {
            --last;
            if (std::forward<Predicate>(predicate)(*last))
                return *last;
        }

        isValid = false;
        return *first;
    }

    template <class ForwardIterator, class Predicate>
    decltype(auto) Last(ForwardIterator first, ForwardIterator last, Predicate predicate, std::forward_iterator_tag, bool& isValid)
    {
        isValid = false;
        first = std::find_if(first, last, predicate);

        if (first == last)
            return *first;

        isValid = true;
        while (true)
        {
            auto before = first;
            first = std::find_if(++first, last, predicate);
            if (first == last)
                return *before;
        }
        return *first;
    }

    template <class InputIterator, class Predicate>
    auto Last(InputIterator first, InputIterator last, Predicate predicate, std::input_iterator_tag, bool& isValid)
    {
        isValid = false;
        first = std::find_if(first, last, predicate);

        if (first == last)
            return *first;

        isValid = true;
        while (true)
        {
            auto before = *first;
            first = std::find_if(++first, last, predicate);
            if (first == last)
                return before;
        }

        return *first;
    }
}
