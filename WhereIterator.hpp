#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Iterator, class Predicate>
    class WhereIterator
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
            : _first(first), _last(last), _predicate(predicate)
        {
            AdvanceUntilFit();
        }

        WhereIterator() = default;
        WhereIterator(WhereIterator const&) = default;
        WhereIterator(WhereIterator&&) = default;
        WhereIterator& operator=(WhereIterator const&) = default;
        WhereIterator& operator=(WhereIterator&&) = default;

    // InputIterator
    public:
        bool operator==(WhereIterator other) const { return _first == other._first; }
        bool operator!=(WhereIterator other) const { return !(*this == other); }
        decltype(auto) operator*() { return *_first; }
        decltype(auto) operator*() const { return *_first; }
        decltype(auto) operator->() { return std::addressof(*_first); }
        decltype(auto) operator->() const { return std::addressof(*_first); }
        WhereIterator& operator++() { ++_first; AdvanceUntilFit(); return *this; }
        WhereIterator operator++(int) { auto copy = *this; ++*this; return copy; }

    // BidirectionalIterator
    public:
        WhereIterator& operator--() { --_first; DecreaseUntilFit(); return *this; }
        WhereIterator operator--(int) { auto copy = *this; --*this; return copy; }

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
