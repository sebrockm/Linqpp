#pragma once

#include <iterator>
#include <algorithm>

namespace Linqpp
{
    namespace Detail
    {
        template <class Iterator>
        auto InternalLast(Iterator first, Iterator last, std::bidirectional_iterator_tag)
        {
            if (first != last)
                return --last;
            return last;
        }

        template <class Iterator>
        auto InternalLast(Iterator first, Iterator last, std::forward_iterator_tag)
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

        template <class Iterator>
        auto InternalLast(Iterator first, Iterator last, std::input_iterator_tag) = delete;

        template <class Iterator, class Predicate>
        auto InternalLast(Iterator first, Iterator last, Predicate predicate, std::bidirectional_iterator_tag)
        {
            auto last2 = last;

            while (first != last)
            {
                --last;
                if (predicate(*last))
                    return last;
            }

            return last2;
        }

        template <class Iterator, class Predicate>
        auto InternalLast(Iterator first, Iterator last, Predicate predicate, std::forward_iterator_tag)
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

        template <class Iterator, class Predicate>
        auto InternalLast(Iterator first, Iterator last, Predicate predicate, std::input_iterator_tag) = delete;
    }

    template <class Iterator>
    auto Last(Iterator first, Iterator last)
    {
        return Detail::InternalLast(first, last, typename std::iterator_traits<Iterator>::iterator_category());
    }

    template <class Iterator, class Predicate>
    auto Last(Iterator first, Iterator last, Predicate predicate)
    {
        return Detail::InternalLast(first, last, predicate, typename std::iterator_traits<Iterator>::iterator_category());
    }
}
