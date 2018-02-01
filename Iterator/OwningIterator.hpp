#pragma once

#include "IteratorAdapter.hpp"

#include <iterator>
#include <limits>
#include <memory>
#include <vector>

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last);

    template <class Iterator>
    class OwningIterator : public IteratorAdapter<OwningIterator<Iterator>>
    {
    // Types
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        using reference = value_type&;
        using pointer = value_type*;

    // Fields
    private:
        Iterator _first;
        Iterator _last;
        std::shared_ptr<std::vector<value_type>> _spData = std::make_shared<std::vector<value_type>>();
        mutable size_t _position = std::numeric_limits<size_t>::max();
        mutable bool _isInitialized = false;

    // Constructors, destructor
    public:
        OwningIterator(Iterator first, Iterator last)
            : _first(first), _last(last), _position(0)
        { }

        OwningIterator() = default;
        OwningIterator(OwningIterator const&) = default;
        OwningIterator(OwningIterator&&) = default;
        OwningIterator& operator=(OwningIterator const&) = default;
        OwningIterator& operator=(OwningIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(OwningIterator const& other) const
        {
            EnsureInitialized();
            other.EnsureInitialized();
            return _spData == other._spData && _position == other._position;
        }

        reference Get() const { EnsureInitialized(); return (*_spData)[_position]; }
        void Increment() { EnsureInitialized(); ++_position; }
        void Decrement() { EnsureInitialized(); --_position; }

        difference_type Difference(OwningIterator const& other) const
        {
            EnsureInitialized();
            other.EnsureInitialized();
            return _position - other._position;
        }

        void Move(difference_type n) { _position += n; }

    // Methods
    public:
        auto GetLast() const
        {
            auto copy = *this;
            copy._position = _isInitialized ? _spData->size() : std::numeric_limits<size_t>::max();
            return copy;
        }

    // Internals
    private:
        void EnsureInitialized() const
        {
            if (_isInitialized)
                return;

            if (_spData->empty())
                _spData->assign(_first, _last);

            if (_position == std::numeric_limits<size_t>::max())
                _position = _spData->size();
            _isInitialized = true;
        }
    };

    template <class InputIterator>
    auto CreateOwningEnumeration(InputIterator first, InputIterator last)
    {
        static_assert(std::is_copy_assignable<OwningIterator<InputIterator>>::value, "OwningIterator is not copy assignable.");

        OwningIterator<InputIterator> firstOwningIterator(first, last);
        return From(firstOwningIterator, firstOwningIterator.GetLast());
    }
}
