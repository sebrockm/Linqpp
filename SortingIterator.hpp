#pragma once

#include "From.hpp"
#include "IteratorAdapter.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace Linqpp
{
    template <class Iterator, class Comparer>
    class SortingIterator : public IteratorAdapter<SortingIterator<Iterator, Comparer>>
    {
        using Vector = std::vector<typename std::iterator_traits<Iterator>::value_type>;
        using Traits = std::iterator_traits<typename Vector::iterator>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = typename Traits::difference_type;
        using value_type = typename Traits::value_type;
        using reference = typename Traits::reference;
        using pointer = typename Traits::pointer;

    // Fields
    private:
        const Iterator _first;
        const Iterator _last;
        const std::shared_ptr<Vector> _spData = std::make_shared<Vector>();
        size_t _position = 0;
        Comparer _comparer;

    // Constructors, destructor
    public:
        SortingIterator(Iterator first, Iterator last, Comparer comparer)
            : _first(first), _last(last), _comparer(comparer)
        { }

        SortingIterator() = default;
        SortingIterator(SortingIterator const&) = default;
        SortingIterator(SortingIterator&&) = default;
        SortingIterator& operator=(SortingIterator const&) = default;
        SortingIterator& operator=(SortingIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(SortingIterator const& other) const { return _spData == other._spData && _position == other._position; }
        reference Get() const { EnsureInitialized(); return (*_spData)[_position]; }
        void Increment() { ++_position; }
        void Decrement() { --_position; }
        difference_type Difference(SortingIterator const& other) const { return _position - other._position; }
        void Move(difference_type n) { _position += n; }

    // Internals
    private:
        void EnsureInitialized() const
        {
            if (_first == _last || !_spData->empty())
                return;

            _spData->assign(_first, _last);
            std::sort(_spData->begin(), _spData->end(), _comparer);
        }
    };

    template <class Iterator, class Comparer>
    auto CreateSortedEnumeration(Iterator first, Iterator last, Comparer comparer)
    {
        auto firstSorted = SortingIterator<Iterator, Comparer>(first, last, comparer);
        return From(firstSorted, firstSorted + From(first, last).Count());
    }
}
