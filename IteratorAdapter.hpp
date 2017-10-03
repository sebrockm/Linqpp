#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Iterator>
    class IteratorAdapter
    {
    private:
        constexpr auto This() const { return static_cast<Iterator const*>(this); }
        constexpr auto This() { return static_cast<Iterator*>(this); }

    // Constructors, destructor
    protected:
        IteratorAdapter() = default;
        IteratorAdapter(IteratorAdapter const&) = default;
        IteratorAdapter(IteratorAdapter&&) = default;
        IteratorAdapter& operator=(IteratorAdapter const&) = default;
        IteratorAdapter& operator=(IteratorAdapter&&) = default;

    // InputIterator
    public:
        bool operator==(Iterator const& other) const { return This()->Equals(other); }
        bool operator!=(Iterator const& other) const { return !(*this == other); }
        decltype(auto) operator*() const { return This()->Get(); }
        decltype(auto) operator->() const { return std::addressof(**this); }
        Iterator& operator++() { This()->Increment(); return *This(); }
        Iterator operator++(int) { auto copy = *This(); ++*this; return copy; }

    // BidirectionalIterator
    public:
        Iterator& operator--() { This()->Decrement(); return *This(); }
        Iterator operator--(int) { auto copy = *This(); --*this; return copy; }

    // RandomAccessIterator
    public:
        template <class difference_type> Iterator& operator+=(difference_type n) { This()->Move(n); return *This(); }
        template <class difference_type> Iterator& operator-=(difference_type n) { This()->Move(-n); return *This(); }
        auto operator-(Iterator const& other) const { return This()->Difference(other); }
        template <class difference_type> decltype(auto) operator[](difference_type n) const { auto copy = *this; copy += n; return *copy; }
        bool operator<(Iterator const& other) const { return (*this - other) < 0; }
        bool operator<=(Iterator const& other) const { return (*this - other) <= 0; }
        bool operator>(Iterator const& other) const { return (*this - other) > 0; }
        bool operator>=(Iterator const& other) const { return (*this - other) >= 0; }
    };

    template <class RandomIterator>
    auto operator+(RandomIterator iterator, typename std::iterator_traits<RandomIterator>::difference_type n) { return iterator += n; }

    template <class RandomIterator>
    auto operator+(typename std::iterator_traits<RandomIterator>::difference_type n, RandomIterator iterator) { return iterator += n; }

    template <class RandomIterator>
    auto operator-(RandomIterator iterator, typename std::iterator_traits<RandomIterator>::difference_type n) { return iterator -= n; }
}
