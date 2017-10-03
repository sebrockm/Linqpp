#pragma once

#include "IteratorAdapter.hpp"

namespace Linqpp
{
    template <class InputIterator, class Predicate>
    class WhereIterator : public IteratorAdapter<WhereIterator<InputIterator, Predicate>>
    {
    // Fields
    private:
        InputIterator _first;
        const InputIterator _last;
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
            : _first(first), _last(last), _predicate(std::move(predicate)) { AdvanceUntilFit(); }

        WhereIterator() = default;
        WhereIterator(WhereIterator const&) = default;
        WhereIterator(WhereIterator&&) = default;
        WhereIterator& operator=(WhereIterator const&) = default;
        WhereIterator& operator=(WhereIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(WhereIterator const& other) const { return _first == other._first; }
        reference Get() const { return *_first; }
        void Increment() { ++_first; AdvanceUntilFit(); }
        void Decrement() { --_first; DecreaseUntilFit(); }

    // Internals
    private:
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
    };

    template <class InputIterator, class Predicate>
    auto CreateWhereIterator(InputIterator first, InputIterator last, Predicate&& predicate)
    {
        return WhereIterator<InputIterator, std::remove_reference_t<Predicate>>(first, last, std::forward<Predicate>(predicate));
    }
}
