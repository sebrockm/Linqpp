#pragma once

#include "IteratorAdapter.hpp"

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
        using reference = value_type&;
        class pointer
        {
            value_type _value;
        public:
            explicit pointer(value_type const& value) : _value(value) { }
            auto operator->() const { return std::addressof(_value); }
            auto operator->() { return std::addressof(_value); }
        };

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
        decltype(auto) Get() { return _function(*_iterator); }
        decltype(auto) Get() const { return _function(*_iterator); }
        decltype(auto) operator->() { return pointer(Get()); }
        decltype(auto) operator->() const { return pointer(Get()); }
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
