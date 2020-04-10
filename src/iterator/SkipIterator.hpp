#pragma once

#include "../From.hpp"
#include "IteratorAdapter.hpp"

#include <algorithm>

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last);

    namespace Detail
    {
        template <class RandomIterator>
        auto InternalGetEnumerableFromSkip(RandomIterator first, size_t n, RandomIterator last, std::random_access_iterator_tag)
        {
            using diff_t = typename std::iterator_traits<RandomIterator>::difference_type;
            return From(first + std::min((diff_t) n, last - first), last);
        }

        template <class InputIterator>
        class SkipIterator : public IteratorAdapter<SkipIterator<InputIterator>>
        {
        private:
            mutable InputIterator _first;
            InputIterator _last;
            mutable bool _initialized = false;
            size_t _skips;

        public:
            using iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;
            using value_type = typename std::iterator_traits<InputIterator>::value_type;
            using difference_type = typename std::iterator_traits<InputIterator>::difference_type;
            using reference = typename std::iterator_traits<InputIterator>::reference;
            using pointer = typename std::iterator_traits<InputIterator>::pointer;

        public:
            SkipIterator(InputIterator first, size_t skips, InputIterator last) : _first(first), _last(last), _skips(skips) { }
            SkipIterator() = default;
            SkipIterator(SkipIterator const&) = default;
            SkipIterator(SkipIterator&&) = default;
            SkipIterator& operator=(SkipIterator const&) = default;
            SkipIterator& operator=(SkipIterator&&) = default;

        public:
            bool Equals(SkipIterator const& other) const 
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
            void Initialize() const
            {
                for (size_t i = 0; i < _skips && _first != _last; ++i)
                    ++_first;

                _initialized = true;
            }
        };

        template <class InputIterator>
        auto InternalGetEnumerableFromSkip(InputIterator first, size_t n, InputIterator last, std::input_iterator_tag)
        {
            static_assert(std::is_copy_assignable<SkipIterator<InputIterator>>::value, "SkipIterator is not copy assignable.");

            return From(SkipIterator<InputIterator>(first, n, last), SkipIterator<InputIterator>(last, n, last));
        }
    }

    template <class InputIterator>
    auto GetEnumerableFromSkip(InputIterator first, size_t n, InputIterator last)
    {
       return Detail::InternalGetEnumerableFromSkip(first, n, last, typename std::iterator_traits<InputIterator>::iterator_category());
    }
}
