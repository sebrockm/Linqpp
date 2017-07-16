#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Subclass>
    class IteratorAdapter
    {
    private:
        constexpr auto This() const { return static_cast<Subclass const*>(this); }
        constexpr auto This() { return static_cast<Subclass*>(this); }

    // Constructors, destructor
    protected:
        IteratorAdapter() = default;
        IteratorAdapter(IteratorAdapter const&) = default;
        IteratorAdapter(IteratorAdapter&&) = default;
        IteratorAdapter& operator=(IteratorAdapter const&) = default;
        IteratorAdapter& operator=(IteratorAdapter&&) = default;

    // InputIterator
    public:
        bool operator==(Subclass const& other) const { return This()->Equals(other); }
        bool operator!=(Subclass const& other) const { return !(*this == other); }
        decltype(auto) operator*() { return This()->Get(); }
        decltype(auto) operator*() const { return This()->Get(); }
        decltype(auto) operator->() { return std::addressof(**this); }
        decltype(auto) operator->() const { return std::addressof(**this); }
        IteratorAdapter& operator++() { This()->Increment(); return *this; }
        IteratorAdapter operator++(int) { auto copy = *this; ++*this; return copy; }

    // BidirectionalIterator
    public:
        IteratorAdapter& operator--() { This()->Decrement(); return *this; }
        IteratorAdapter operator--(int) { auto copy = *this; --*this; return copy; }

    // RandomAccessIterator
    public:
        template <class difference_type> IteratorAdapter& operator+=(difference_type n) { This()->Move(n); return *this; }
        template <class difference_type> IteratorAdapter& operator-=(difference_type n) { This()->Move(-n); return *this; }
        auto operator-(Subclass const& other) const { return This()->Difference(other); }
        template <class difference_type> decltype(auto) operator[](difference_type n) { auto copy = *this; copy += n; return *copy; }
        template <class difference_type> decltype(auto) operator[](difference_type n) const { auto copy = *this; copy += n; return *copy; }
        bool operator<(Subclass const& other) const { return (*this - other) < 0; }
        bool operator<=(Subclass const& other) const { return (*this - other) <= 0; }
        bool operator>(Subclass const& other) const { return (*this - other) > 0; }
        bool operator>=(Subclass const& other) const { return (*this - other) >= 0; }
    };

    template <class Iterator>
    auto operator+(Iterator iterator, typename std::iterator_traits<Iterator>::difference_type n) { return iterator += n; }

    template <class Iterator>
    auto operator+(typename std::iterator_traits<Iterator>::difference_type n, Iterator iterator) { return iterator += n; }

    template <class Iterator>
    auto operator-(Iterator iterator, typename std::iterator_traits<Iterator>::difference_type n) { return iterator -= n; }
}
