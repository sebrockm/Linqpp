#pragma once

#include "IteratorAdapter.hpp"

namespace Linqpp
{
    template <class InputIterator, class Predicate>
    class WhereIterator : public IteratorAdapter<WhereIterator<InputIterator, Predicate>>
    {
    // Fields
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
        using difference_type = typename std::iterator_traits<InputIterator>::difference_type;
        using value_type = typename std::iterator_traits<InputIterator>::value_type;
        using reference = typename std::iterator_traits<InputIterator>::reference;
        using pointer = typename std::iterator_traits<InputIterator>::pointer;

    // Constructors, destructor
    public:
        WhereIterator(InputIterator first, InputIterator last, Predicate predicate)
            : _first(first), _last(last), _predicate(std::move(predicate)) { }

        WhereIterator() = default;
        WhereIterator(WhereIterator const&) = default;
        WhereIterator(WhereIterator&&) = default;

        WhereIterator& operator=(WhereIterator other)
        {
            swap(*this, other);
            return *this;
        }

    // IteratorAdapter
    public:
        bool Equals(WhereIterator const& other) const
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
            AdvanceUntilFit();
        }

        void Decrement()
        {
            if (!_isInitialized)
                Initialize();

            --_first;
            DecreaseUntilFit();
        }

    // Internals
    private:
        void Initialize() const
        {
            for (; _first != _last && !_predicate(*_first); ++_first);
            _isInitialized = true;
        }

        void AdvanceUntilFit()
        {
            while (_first != _last && !_predicate(*_first))
                ++_first;
        }

        void DecreaseUntilFit()
        {
            while (!_predicate(*_first))
                --_first;
        }

        friend void swap(WhereIterator& iterator1, WhereIterator& iterator2)
        {
            Swap(iterator1, iterator2, std::is_copy_assignable<Predicate>());
        }

        static void Swap(WhereIterator& iterator1, WhereIterator& iterator2, std::true_type)
        {
            using std::swap;
            Swap(iterator1, iterator2, std::false_type());
            swap(iterator1._predicate, iterator2._predicate);
        }

        static void Swap(WhereIterator& iterator1, WhereIterator& iterator2, std::false_type)
        {
            using std::swap;
            swap(iterator1._first, iterator2._first);
            swap(iterator1._last, iterator2._last);
            swap(iterator1._isInitialized, iterator2._isInitialized);
        }
    };

    template <class InputIterator, class Predicate>
    auto CreateWhereIterator(InputIterator first, InputIterator last, Predicate predicate)
    {
        static_assert(std::is_copy_assignable<WhereIterator<InputIterator, Predicate>>::value, "WhereIterator is not copy assignable.");

        return WhereIterator<InputIterator, std::remove_reference_t<Predicate>>(first, last, predicate);
    }
}
