#pragma once

#include "From.hpp"
#include "IteratorAdapter.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace Linqpp
{
    template <class InputIterator, class LessThanComparer>
    class SortingIterator : public IteratorAdapter<SortingIterator<InputIterator, LessThanComparer>>
    {
        using Vector = std::vector<typename std::iterator_traits<InputIterator>::value_type>;
        using Traits = std::iterator_traits<typename Vector::iterator>;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = typename Traits::difference_type;
        using value_type = typename Traits::value_type;
        using reference = typename Traits::reference;
        using pointer = typename Traits::pointer;

    // Fields
    private:
        InputIterator _first;
        InputIterator _last;
        std::shared_ptr<Vector> _spData = std::make_shared<Vector>();
        size_t _position = 0;
        LessThanComparer _comparer;

    // Constructors, destructor
    public:
        SortingIterator(InputIterator first, InputIterator last, LessThanComparer comparer)
            : _first(first), _last(last), _comparer(std::move(comparer))
        { }

        SortingIterator() = default;
        SortingIterator(SortingIterator const&) = default;
        SortingIterator(SortingIterator&&) = default;

        SortingIterator& operator=(SortingIterator other)
        {
            Swap(*this, other, std::is_copy_assignable<LessThanComparer>());
            return *this;
        }

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

        static void Swap(SortingIterator& iterator1, SortingIterator& iterator2, std::true_type)
        {
            Swap(iterator1, iterator2, std::false_type());
            std::swap(iterator1._comparer, iterator2._comparer);
        }

        static void Swap(SortingIterator& iterator1, SortingIterator& iterator2, std::false_type)
        {
            std::swap(iterator1._first, iterator2._first);
            std::swap(iterator1._last, iterator2._last);
            std::swap(iterator1._spData, iterator2._spData);
            std::swap(iterator1._position, iterator2._position);
        }
    };

    template <class InputIterator, class LessThanComparer>
    auto CreateSortedEnumeration(InputIterator first, InputIterator last, LessThanComparer&& comparer)
    {
        static_assert(std::is_copy_assignable<SortingIterator<InputIterator, std::remove_reference_t<LessThanComparer>>>::value, "SortingIterator is not copy assignable.");

        auto firstSorted = SortingIterator<InputIterator, std::remove_reference_t<LessThanComparer>>(first, last, std::forward<LessThanComparer>(comparer));
        return From(firstSorted, firstSorted + From(first, last).Count());
    }
}
