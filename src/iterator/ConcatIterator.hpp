#pragma once

#include "IteratorAdapter.hpp"

namespace Linqpp
{
    template <class Iterator1, class Iterator2>
    class ConcatIterator : public IteratorAdapter<ConcatIterator<Iterator1, Iterator2>>
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

    private:
        Iterator1 _last1;
        Iterator2 _first2;
        Iterator1 _current1;
        Iterator2 _current2;

    public:
        using iterator_category = std::common_type_t<iterator_category1, iterator_category2>;
        using value_type = std::common_type_t<value_type1, value_type2>;
        using difference_type = std::common_type_t<difference_type1, difference_type2>;
        using reference = std::common_type_t<reference1, reference2>;
        using pointer = std::common_type_t<pointer1, pointer2>;

    public:
        ConcatIterator(Iterator1 current1, Iterator1 last1, Iterator2 first2, Iterator2 current2)
            : _last1(last1), _first2(first2), _current1(current1), _current2(current2)
        { }

        ConcatIterator() = default;
        ConcatIterator(ConcatIterator const&) = default;
        ConcatIterator(ConcatIterator&&) = default;
        ConcatIterator& operator=(ConcatIterator const&) = default;
        ConcatIterator& operator=(ConcatIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(ConcatIterator const& other) const { return _current1 == other._current1 && _current2 == other._current2; }
        reference Get() const { return (_current1 != _last1) ? *_current1 : *_current2; }
        void Increment() { if (_current1 != _last1) ++_current1; else ++_current2; }
        void Decrement() { if (_current2 == _first2) --_current1; else --_current2; }

        difference_type Difference(ConcatIterator const& other) const { return (_current1 - other._current1) + (_current2 - other._current2); }

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
    auto CreateConcatIterator(Iterator1 current1, Iterator1 last1, Iterator2 first2, Iterator2 current2)
    {
        static_assert(std::is_copy_assignable<ConcatIterator<Iterator1, Iterator2>>::value, "ConcatIterator is not copy assignable.");

        return ConcatIterator<Iterator1, Iterator2>(current1, last1, first2, current2);
    }
}
