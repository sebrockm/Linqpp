#pragma once

#include "../From.hpp"
#include "IteratorAdapter.hpp"

#include <algorithm>

namespace Linqpp
{
    namespace Detail
    {
        template <class InputIterator, class Predicate>
        class SkipWhileIterator : public IteratorAdapter<SkipWhileIterator<InputIterator, Predicate>>
        {
        private:
            mutable InputIterator _first;
            InputIterator _last;
            mutable bool _initialized = false;
            Predicate _predicate;

        public:
            using iterator_category = std::conditional_t<
                std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<InputIterator>::iterator_category>::value,
                std::bidirectional_iterator_tag,
                typename std::iterator_traits<InputIterator>::iterator_category>;
            using value_type = typename std::iterator_traits<InputIterator>::value_type;
            using difference_type = typename std::iterator_traits<InputIterator>::difference_type;
            using reference = typename std::iterator_traits<InputIterator>::reference;
            using pointer = typename std::iterator_traits<InputIterator>::pointer;

        public:
            SkipWhileIterator(InputIterator first, InputIterator last, Predicate predicate) : _first(first), _last(last), _predicate(predicate) { }
            SkipWhileIterator() = default;
            SkipWhileIterator(SkipWhileIterator const&) = default;
            SkipWhileIterator(SkipWhileIterator&&) = default;
            SkipWhileIterator& operator=(SkipWhileIterator const&) = default;
            SkipWhileIterator& operator=(SkipWhileIterator&&) = default;

        public:
            bool Equals(SkipWhileIterator const& other) const 
            { 
                if (!_initialized)
                    Initialize();

                return _first == other._first;
            }

            reference Get() const
            {
                if (!_initialized)
                    Initialize();

                return *_first;
            }

            void Increment()
            {
                if (!_initialized)
                    Initialize();

                ++_first;
            }

            void Decrement()
            {
                if (!_initialized)
                    Initialize();

                --_first;
            }

        private:
            template <class P = Predicate>
            void Initialize(decltype(std::declval<P>()(*_first))* = nullptr) const
            {
                for (; _first != _last && _predicate(*_first); ++_first);

                _initialized = true;
            }

            template <class P = Predicate>
            void Initialize(decltype(std::declval<P>()(*_first, 0))* = nullptr) const
            {
                for (size_t i = 0; _first != _last && _predicate(*_first, i); ++i, ++_first);

                _initialized = true;
            }
        };
    }

    template <class InputIterator, class Predicate>
    auto GetEnumerableFromSkipWhile(InputIterator first, InputIterator last, Predicate predicate)
    {
        return From(SkipWhileIterator<InputIterator, Predicate>(first, last, predicate), SkipWhileIterator<InputIterator, Predicate>(last, last, predicate));
    }
}
