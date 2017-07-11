#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Iterator, class Function>
    class SelectIterator
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

    // InputIterator
    public:
        bool operator==(SelectIterator other) const { return _iterator == other._iterator; }
        bool operator!=(SelectIterator other) const { return !(*this == other); }
        decltype(auto) operator*() { return _function(*_iterator); }
        decltype(auto) operator*() const { return _function(*_iterator); }
        decltype(auto) operator->() { return pointer(**this); }
        decltype(auto) operator->() const { return pointer(**this); }
        SelectIterator& operator++() { ++_iterator; return *this; }
        SelectIterator operator++(int) { auto copy = *this; ++*this; return copy; }

    // BidirectionalIterator
    public:
        SelectIterator& operator--() { --_iterator; return *this; }
        SelectIterator operator--(int) { auto copy = *this; --*this; return copy; }

    // RandomAccessIterator
    public:
        SelectIterator& operator+=(difference_type n) { _iterator += n; return *this; }
        SelectIterator& operator-=(difference_type n) { _iterator -= n; return *this; }
        difference_type operator-(SelectIterator other) const { return _iterator - other._iterator; }
        decltype(auto) operator[](difference_type n) { return *(*this + n); }
        decltype(auto) operator[](difference_type n) const { return *(*this + n); }
        bool operator<(SelectIterator other) const { return (*this - other) < 0; }
        bool operator<=(SelectIterator other) const { return (*this - other) <= 0; }
        bool operator>(SelectIterator other) const { return (*this - other) > 0; }
        bool operator>=(SelectIterator other) const { return (*this - other) >= 0; }
    };

    template <class Iterator, class Function>
    auto operator+(SelectIterator<Iterator, Function> iterator, typename std::iterator_traits<decltype(iterator)>::difference_type n)
    {
        return iterator += n;
    }

    template <class Iterator, class Function>
    auto operator+(typename std::iterator_traits<SelectIterator<Iterator, Function>>::difference_type n, SelectIterator<Iterator, Function> iterator)
    {
        return iterator += n;
    }

    template <class Iterator, class Function>
    auto operator-(SelectIterator<Iterator, Function> iterator, typename std::iterator_traits<decltype(iterator)>::difference_type n)
    {
        return iterator -= n;
    }

    template <class Iterator, class Function>
    auto CreateSelectIterator(Iterator iterator, Function function)
    {
        return SelectIterator<Iterator, Function>(iterator, function);
    }
}
