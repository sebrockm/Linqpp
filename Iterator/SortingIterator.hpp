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
        mutable size_t _position = std::numeric_limits<size_t>::max();
        LessThanComparer _comparer;
        mutable bool _isInitialized = false;

    // Constructors, destructor
    public:
        SortingIterator(InputIterator first, InputIterator last, LessThanComparer comparer)
            : _first(first), _last(last), _position(0), _comparer(std::move(comparer))
        { }

        SortingIterator() = default;
        SortingIterator(SortingIterator const&) = default;
        SortingIterator(SortingIterator&&) = default;

        SortingIterator& operator=(SortingIterator other)
        {
            swap(*this, other);
            return *this;
        }

    // IteratorAdapter
    public:
        bool Equals(SortingIterator const& other) const
        {
            EnsureInitialized();
            other.EnsureInitialized();
            return _spData == other._spData && _position == other._position;
        }

        reference Get() const { EnsureInitialized(); return (*_spData)[_position]; }
        void Increment() { EnsureInitialized(); ++_position; }
        void Decrement() { EnsureInitialized(); --_position; }

        difference_type Difference(SortingIterator const& other) const
        {
            EnsureInitialized();
            other.EnsureInitialized();
            return _position - other._position;
        }

        void Move(difference_type n) { EnsureInitialized(); _position += n; }

        auto GetEnd() const
        {
            auto end = *this;
            end._position = _isInitialized ? _spData->size() : std::numeric_limits<size_t>::max();
            return end;
        }

    // Internals
    private:
        void EnsureInitialized() const
        {
            if (_isInitialized)
                return;

            if (_spData->empty())
            {
                *_spData = From(_first, _last).ToVector();
                std::sort(_spData->begin(), _spData->end(), _comparer);
            }

            if (_position == std::numeric_limits<size_t>::max())
                _position = _spData->size();

            _isInitialized = true;
        }

        friend void swap(SortingIterator& iterator1, SortingIterator& iterator2)
        {
            Swap(iterator1, iterator2, std::is_copy_assignable<LessThanComparer>());
        }

        static void Swap(SortingIterator& iterator1, SortingIterator& iterator2, std::true_type)
        {
            using std::swap;
            Swap(iterator1, iterator2, std::false_type());
            swap(iterator1._comparer, iterator2._comparer);
        }

        static void Swap(SortingIterator& iterator1, SortingIterator& iterator2, std::false_type)
        {
            using std::swap;
            swap(iterator1._first, iterator2._first);
            swap(iterator1._last, iterator2._last);
            swap(iterator1._spData, iterator2._spData);
            swap(iterator1._position, iterator2._position);
            swap(iterator1._isInitialized, iterator2._isInitialized);
        }
    };

    template <class InputIterator, class LessThanComparer>
    auto CreateSortedEnumerable(InputIterator first, InputIterator last, LessThanComparer&& comparer)
    {
        static_assert(std::is_copy_assignable<SortingIterator<InputIterator, std::remove_reference_t<LessThanComparer>>>::value, "SortingIterator is not copy assignable.");

        auto firstSorted = SortingIterator<InputIterator, std::remove_reference_t<LessThanComparer>>(first, last, std::forward<LessThanComparer>(comparer));
        return From(firstSorted, firstSorted.GetEnd());
    }
}
