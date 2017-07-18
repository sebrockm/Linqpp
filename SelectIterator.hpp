#pragma once

#include "IteratorAdapter.hpp"
#include "DummyPointer.hpp"

using namespace Linqpp::Detail;

namespace Linqpp
{
    template <class Iterator, class Function>
    class SelectIterator : public IteratorAdapter<SelectIterator<Iterator, Function>>
    {
    // Fields
    private:
        Iterator _iterator;
        Function _function;

    public:
        using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
        using difference_type = typename std::iterator_traits<Iterator>::difference_type;
        using value_type = decltype(_function(*_iterator));
        using reference = value_type;
        using pointer = DummyPointer<value_type>;

    // Constructors, destructor
    public:
        SelectIterator(Iterator iterator, Function function)
            : _iterator(iterator), _function(function)
        { }

        SelectIterator() = default;
        SelectIterator(SelectIterator const&) = default;
        SelectIterator(SelectIterator&&) = default;
        SelectIterator& operator=(SelectIterator const&) = default;
        SelectIterator& operator=(SelectIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(SelectIterator const& other) const { return _iterator == other._iterator; }
        reference Get() { return _function(*_iterator); }
        reference Get() const { return _function(*_iterator); }
        pointer operator->() { return CreateDummyPointer(Get()); }
        pointer operator->() const { return CreateDummyPointer(Get()); }
        void Increment() { ++_iterator; }
        void Decrement() { --_iterator; }
        difference_type Difference(SelectIterator const& other) const { return _iterator - other._iterator; }
    };

    template <class Iterator, class Function>
    auto CreateSelectIterator(Iterator iterator, Function function)
    {
        return SelectIterator<Iterator, Function>(iterator, function);
    }
}
