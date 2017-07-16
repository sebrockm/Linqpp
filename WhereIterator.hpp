#pragma once

#include "IteratorAdapter.hpp"

namespace Linqpp
{
    template <class Iterator, class Predicate>
    class WhereIterator : public IteratorAdapter<WhereIterator<Iterator, Predicate>>
    {
    // Fields
    private:
        Iterator _first;
        const Iterator _last;
        Predicate _predicate;

    public:
        using iterator_category = std::conditional_t<
            std::is_base_of<std::bidirectional_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category>::value,
                std::bidirectional_iterator_tag,
                typename std::iterator_traits<Iterator>::iterator_category>;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using reference = typename std::iterator_traits<Iterator>::reference;
        using pointer = typename std::iterator_traits<Iterator>::pointer;

    // Constructors, destructor
    public:
        WhereIterator(Iterator first, Iterator last, Predicate predicate)
            : _first(first), _last(last), _predicate(predicate) { AdvanceUntilFit(); }

        WhereIterator() = default;
        WhereIterator(WhereIterator const&) = default;
        WhereIterator(WhereIterator&&) = default;
        WhereIterator& operator=(WhereIterator const&) = default;
        WhereIterator& operator=(WhereIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(WhereIterator const& other) const { return _first == other._first; }
        decltype(auto) Get() { return *_first; }
        decltype(auto) Get() const { return *_first; }
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

    template <class Iterator, class Predicate>
    auto CreateWhereIterator(Iterator first, Iterator last, Predicate predicate)
    {
        return WhereIterator<Iterator, Predicate>(first, last, predicate);
    }
}
