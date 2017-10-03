#pragma once

#include <iterator>
#include <algorithm>

namespace Linqpp
{
    namespace Detail
    {
        template <class BidirectionalIterator>
        auto InternalLast(BidirectionalIterator first, BidirectionalIterator last, std::bidirectional_iterator_tag)
        {
            if (first != last)
                return --last;
            return last;
        }

        template <class ForwardIterator>
        auto InternalLast(ForwardIterator first, ForwardIterator last, std::forward_iterator_tag)
        {
            if (first == last)
                return first;

            while (true)
            {
                auto before = first;
                if (++first == last)
                    return before;
            }
        }

        template <class InputIterator>
        auto InternalLast(InputIterator first, InputIterator last, std::input_iterator_tag) = delete;

        template <class BidirectionalIterator, class Predicate>
        auto InternalLast(BidirectionalIterator first, BidirectionalIterator last, Predicate&& predicate, std::bidirectional_iterator_tag)
        {
            auto last2 = last;

            while (first != last)
            {
                --last;
                if (std::forward<Predicate>(predicate)(*last))
                    return last;
            }

            return last2;
        }

        template <class ForwardIterator, class Predicate>
        auto InternalLast(ForwardIterator first, ForwardIterator last, Predicate predicate, std::forward_iterator_tag)
        {
            first = std::find_if(first, last, predicate);
            if (first == last)
                return first;

            while (true)
            {
                auto before = first;
                first = std::find_if(first, last, predicate);
                if (first == last)
                    return before;
            }
        }

        template <class InputIterator, class Predicate>
        auto InternalLast(InputIterator first, InputIterator last, Predicate predicate, std::input_iterator_tag) = delete;
    }

    template <class ForwardIterator>
    auto Last(ForwardIterator first, ForwardIterator last)
    {
        return Detail::InternalLast(first, last, typename std::iterator_traits<ForwardIterator>::iterator_category());
    }

    template <class ForwardIterator, class Predicate>
    auto Last(ForwardIterator first, ForwardIterator last, Predicate&& predicate)
    {
        return Detail::InternalLast(first, last, std::forward<Predicate>(predicate), typename std::iterator_traits<ForwardIterator>::iterator_category());
    }
}
