#pragma once

#include <iterator>

namespace Linqpp
{
    template <class Iterator1, class Iterator2>
    class ConcatIterator
    {
        using value_type1 = typename std::iterator_traits<Iterator1>::value_type;
        using value_type2 = typename std::iterator_traits<Iterator2>::value_type;
        using difference_type1 = typename std::iterator_traits<Iterator1>::difference_type;
        using difference_type2 = typename std::iterator_traits<Iterator2>::difference_type;
        using reference1 = typename std::iterator_traits<Iterator1>::reference;
        using reference2 = typename std::iterator_traits<Iterator2>::reference;
        using pointer1 = typename std::iterator_traits<Iterator1>::pointer;
        using pointer2 = typename std::iterator_traits<Iterator2>::pointer;
        using iterator_category1 = typename std::iterator_traits<Iterator1>::iterator_category;
        using iterator_category2 = typename std::iterator_traits<Iterator2>::iterator_category;

        static_assert(std::is_same<value_type1, value_type2>::value, "Iterators must have the same value_type.");
        static_assert(std::is_same<difference_type1, difference_type2>::value, "Iterators must have the same difference_type.");
        static_assert(std::is_same<reference1, reference2>::value, "Iterators must have the same reference.");
        static_assert(std::is_same<pointer1, pointer2>::value, "Iterators must have the same pointer.");

    private:
        Iterator1 _last1;
        Iterator2 _first2;
        Iterator1 _current1;
        Iterator2 _current2;

    public:
        using iterator_category = std::common_type_t<iterator_category1, iterator_category2>;
        using value_type = value_type1;
        using difference_type = difference_type1;
        using reference = reference1;
        using pointer = pointer1;

    public:
        ConcatIterator(Iterator1 current1, Iterator1 last1, Iterator2 first2, Iterator2 current2)
            : _last1(last1), _first2(first2), _current1(current1), _current2(current2)
        { }

        ConcatIterator() = default;
        ConcatIterator(ConcatIterator const&) = default;
        ConcatIterator& operator=(ConcatIterator const&) = default;

    // InputIterator
    public:
        bool operator==(ConcatIterator other) const { return _current1 == other._current1 && _current2 == other._current2; }
        bool operator!=(ConcatIterator other) const { return !(*this == other); }
        decltype(auto) operator*() { return (_current1 != _last1) ? *_current1 : *_current2; }
        decltype(auto) operator*() const { return (_current1 != _last1) ? *_current1 : *_current2; }
        decltype(auto) operator->() { return std::addressof(**this); }
        decltype(auto) operator->() const { return std::addressof(**this); }
        ConcatIterator& operator++() { if (_current1 != _last1) ++_current1; else ++_current2; return *this; }
        ConcatIterator operator++(int) { auto copy = *this; ++*this; return copy; }

    // BidirectionalIterator
    public:
        ConcatIterator& operator--() { if (_current2 == _first2) --_current1; else --_current2; return *this; }
        ConcatIterator operator--(int) { auto copy = *this; --*this; return copy; }

    // RandomAccessIterator
    public:
        ConcatIterator& operator+=(difference_type n) { Move(n); return *this; }
        ConcatIterator& operator-=(difference_type n) { Move(-n); return *this; }
        difference_type operator-(ConcatIterator other) const { return (_current1 - other._current1) + (_current2 - other._current2); }
        decltype(auto) operator[](difference_type n) { return *(*this + n); }
        decltype(auto) operator[](difference_type n) const { return *(*this + n); }
        bool operator<(ConcatIterator other) const { return (*this - other) < 0; }
        bool operator<=(ConcatIterator other) const { return (*this - other) <= 0; }
        bool operator>(ConcatIterator other) const { return (*this - other) > 0; }
        bool operator>=(ConcatIterator other) const { return (*this - other) >= 0; }

    // Internal
    private:
        void Move(difference_type n)
        {
            if (n > 0)
            {
                auto diff1 = _last1 - _current1;
                if (n <= diff1)
                {
                    _current1 += n;
                }
                else
                {
                    _current1 = _last1;
                    _current2 += (n - diff1);
                }
            }
            else if (n < 0)
            {
                auto diff2 = _current2 - _first2;
                if (-n <= diff2)
                {
                    _current2 += n;
                }
                else
                {
                    _current2 = _first2;
                    _current1 += (n + diff2);
                }
            }
        }
    };

    template <class Iterator1, class Iterator2>
    auto operator+(ConcatIterator<Iterator1, Iterator2> iter, typename ConcatIterator<Iterator1, Iterator2>::difference_type n) { return iter += n; }

    template <class Iterator1, class Iterator2>
    auto operator+(typename ConcatIterator<Iterator1, Iterator2>::difference_type n, ConcatIterator<Iterator1, Iterator2> iter) { return iter += n; }

    template <class Iterator1, class Iterator2>
    auto operator-(ConcatIterator<Iterator1, Iterator2> iter, typename ConcatIterator<Iterator1, Iterator2>::difference_type n) { return iter -= n; }

    template <class Iterator1, class Iterator2>
    auto CreateConcatIterator(Iterator1 current1, Iterator1 last1, Iterator2 first2, Iterator2 current2)
    {
        return ConcatIterator<Iterator1, Iterator2>(current1, last1, first2, current2);
    }
}
