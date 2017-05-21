#pragma once

#include "Enumeration.hpp"

#include <algorithm>

namespace Linqpp
{
    template <class Iterator>
    class Enumeration;

    template <class Iterator>
    auto From(Iterator first, Iterator last);

    namespace Detail
    {
        template <class Iterator>
        auto InternalGetEnumeratorFromTake(Iterator first, size_t n, Iterator last, std::random_access_iterator_tag)
        {
           return From(first, std::min(first + n, last));
        }

        template <class Iterator, class Category>
        auto InternalGetEnumeratorFromTake(Iterator first, size_t n, Iterator last, Category)
        {
           using TakeCategory = std::conditional_t<std::is_same<Category, std::bidirectional_iterator_tag>::value, std::forward_iterator_tag, Category>;

           class TakeIterator : public std::iterator<TakeCategory, typename std::iterator_traits<Iterator>::value_type>
           {
           private:
              Iterator _first;
              Iterator _last;
              size_t _position;

           public:
              TakeIterator(Iterator first, size_t position, Iterator last) : _first(first), _last(last), _position(position) { }

           public:
              decltype(auto) operator*() const { return *_first; }
              decltype(auto) operator*() { return *_first; }
              decltype(auto) operator->() const { return std::addressof(*_first); }
              decltype(auto) operator->() { return std::addressof(*_first); }
              TakeIterator& operator++() { Advance(); return *this; }
              TakeIterator operator++(int) { auto copy = *this; Advance(); return copy; }
              bool operator==(TakeIterator const& other) const { return _last == other._last && (_first == other._first || _position == other._position); }
              bool operator!=(TakeIterator const& other) const { return !(*this == other); }

           private:
              void Advance() { ++_first; ++_position; }
           };

           return From(TakeIterator(first, 0, last), TakeIterator(last, n, last));
        }
    }

    template <class Iterator, class Category = typename std::iterator_traits<Iterator>::iterator_category>
    auto GetEnumeratorFromTake(Iterator first, size_t n, Iterator last)
    {
       return Detail::InternalGetEnumeratorFromTake(first, n, last, Category());
    }
}
