#pragma once

#include "../From.hpp"
#include "IteratorAdapter.hpp"

#include <algorithm>

namespace Linqpp
{
    template <class InputIterator, class Predicate>
    class SkipWhileIterator : public IteratorAdapter<SkipWhileIterator<InputIterator, Predicate>>
    {
    private:
        mutable InputIterator _first;
        InputIterator _last;
        mutable bool _isInitialized = false;
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

        SkipWhileIterator& operator=(SkipWhileIterator other)
        {
            swap(*this, other);
            return *this;
        }

    public:
        bool Equals(SkipWhileIterator const& other) const 
        { 
            if (!_isInitialized)
                Initialize();

            if (!other._isInitialized)
                other.Initialize();

            return _first == other._first;
        }

        reference Get() const
        {
            if (!_isInitialized)
                Initialize();

            return *_first;
        }

        void Increment()
        {
            if (!_isInitialized)
                Initialize();

            ++_first;
        }

        void Decrement()
        {
            if (!_isInitialized)
                Initialize();

            --_first;
        }

    private:
        template <class P = Predicate>
        void Initialize(decltype(std::declval<P>()(*_first))* = nullptr) const
        {
            for (; _first != _last && _predicate(*_first); ++_first);

            _isInitialized = true;
        }

        template <class P = Predicate>
        void Initialize(decltype(std::declval<P>()(*_first, 0))* = nullptr) const
        {
            for (size_t i = 0; _first != _last && _predicate(*_first, i); ++i, ++_first);

            _isInitialized = true;
        }

        friend void swap(SkipWhileIterator& iterator1, SkipWhileIterator& iterator2)
        {
            Swap(iterator1, iterator2, std::is_copy_assignable<Predicate>());
        }

        static void Swap(SkipWhileIterator& iterator1, SkipWhileIterator& iterator2, std::true_type)
        {
            using std::swap;
            Swap(iterator1, iterator2, std::false_type());
            swap(iterator1._predicate, iterator2._predicate);
        }

        static void Swap(SkipWhileIterator& iterator1, SkipWhileIterator& iterator2, std::false_type)
        {
            using std::swap;
            swap(iterator1._first, iterator2._first);
            swap(iterator1._last, iterator2._last);
            swap(iterator1._isInitialized, iterator2._isInitialized);
        }
    };

    template <class InputIterator, class Predicate>
    auto GetEnumerableFromSkipWhile(InputIterator first, InputIterator last, Predicate predicate)
    {
        return From(SkipWhileIterator<InputIterator, Predicate>(first, last, predicate), SkipWhileIterator<InputIterator, Predicate>(last, last, predicate));
    }
}
