#pragma once

#include "IteratorAdapter.hpp"

#include <iterator>
#include <memory>
#include <vector>

namespace Linqpp
{
    template <class Type>
    class OwningIterator : public IteratorAdapter<OwningIterator<Type>>
    {
    // Fields
    private:
        std::shared_ptr<std::vector<Type>> _spContent;
        size_t _position;

    // Types
    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Type
        using reference = Type&;
        using pointer = Type*;

    // Constructors, destructor
    public:
        template <class Iterator>
        OwningIterator(Iterator first, Iterator last)
            : _spContent(std::make_shared<std::vector<Type>>(first, last)), _position(0)
        { }

        OwningIterator() = default;
        OwningIterator(OwningIterator const&) = default;
        OwningIterator(OwningIterator&&) = default;
        OwningIterator& operator=(OwningIterator const&) = default;
        OwningIterator& operator=(OwningIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(OwningIterator const& other) const { return _spContent == other._spContent && _position == other._position; }
        reference Get() const { return (*_spContent)[_position]; }
        void Increment() { ++_position; }
        void Decrement() { --_position; }
        difference_type Difference(OwningIterator const& other) const { return _position - other._position; }
    };

    template <class Iterator>
    auto CreateOwningIterator(Iterator first, Iterator last)
    {
        return OwningIterator<typename std::iterator_traits<Iterator>::value_type>(first, last);
    }
}
