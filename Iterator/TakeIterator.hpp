#pragma once

#include "From.hpp"
#include "IteratorAdapter.hpp"

#include <algorithm>

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last);

    namespace Detail
    {
        template <class RandomIterator>
        auto InternalGetEnumeratorFromTake(RandomIterator first, size_t n, RandomIterator last, std::random_access_iterator_tag)
        {
           return From(first, std::min(first + n, last));
        }

        template <class InputIterator, class Category>
        class TakeIterator : public IteratorAdapter<TakeIterator<InputIterator, Category>>
        {
        private:
            InputIterator _first;
            InputIterator _last;
            size_t _position;

        public:
            using iterator_category = std::conditional_t<std::is_base_of<std::bidirectional_iterator_tag, Category>::value, 
                std::forward_iterator_tag, Category>;
            using value_type = typename std::iterator_traits<InputIterator>::value_type;
            using difference_type = typename std::iterator_traits<InputIterator>::difference_type;
            using reference = typename std::iterator_traits<InputIterator>::reference;
            using pointer = typename std::iterator_traits<InputIterator>::pointer;

        public:
            TakeIterator(InputIterator first, size_t position, InputIterator last) : _first(first), _last(last), _position(position) { }
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

        template <class InputIterator>
        auto InternalGetEnumeratorFromTake(InputIterator first, size_t n, InputIterator last, std::input_iterator_tag)
        {
            using Category = typename std::iterator_traits<InputIterator>::iterator_category;
            static_assert(std::is_copy_assignable<TakeIterator<InputIterator, Category>>::value, "TakeIterator is not copy assignable.");

            return From(TakeIterator<InputIterator, Category>(first, 0, last), TakeIterator<InputIterator, Category>(last, n, last));
        }
    }

    template <class InputIterator>
    auto GetEnumeratorFromTake(InputIterator first, size_t n, InputIterator last)
    {
       return Detail::InternalGetEnumeratorFromTake(first, n, last, typename std::iterator_traits<InputIterator>::iterator_category());
    }
}
