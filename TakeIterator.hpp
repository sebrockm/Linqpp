#pragma once

#include "Enumerable.hpp"
#include "IteratorAdapter.hpp"

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
        class TakeIterator : public IteratorAdapter<TakeIterator<Iterator, Category>>
        {
        private:
            Iterator _first;
            Iterator _last;
            size_t _position;

        public:
            using iterator_category = std::conditional_t<std::is_base_of<std::bidirectional_iterator_tag, Category>::value, 
                std::forward_iterator_tag, Category>;
            using value_type = typename std::iterator_traits<Iterator>::value_type;
            using difference_type = typename std::iterator_traits<Iterator>::difference_type;
            using reference = typename std::iterator_traits<Iterator>::reference;
            using pointer = typename std::iterator_traits<Iterator>::pointer;

        public:
            TakeIterator(Iterator first, size_t position, Iterator last) : _first(first), _last(last), _position(position) { }
            TakeIterator() = default;
            TakeIterator(TakeIterator const&) = default;
            TakeIterator(TakeIterator&&) = default;
            TakeIterator& operator=(TakeIterator const&) = default;
            TakeIterator& operator=(TakeIterator&&) = default;

        public:
            bool Equals(TakeIterator const& other) const 
            { 
                return _last == other._last && (_first == other._first || _position == other._position);
            }

            reference Get() const { return *_first; }
            void Increment() { ++_first; ++_position; }
        };

        template <class Iterator, class Category>
        auto InternalGetEnumeratorFromTake(Iterator first, size_t n, Iterator last, Category)
        {
           return From(TakeIterator<Iterator, Category>(first, 0, last), TakeIterator<Iterator, Category>(last, n, last));
        }
    }

    template <class Iterator, class Category = typename std::iterator_traits<Iterator>::iterator_category>
    auto GetEnumeratorFromTake(Iterator first, size_t n, Iterator last)
    {
       return Detail::InternalGetEnumeratorFromTake(first, n, last, Category());
    }
}
