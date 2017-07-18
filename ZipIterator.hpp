#pragma once

#include "IteratorAdapter.hpp"
#include "DummyPointer.hpp"

using namespace Linqpp::Detail;

namespace Linqpp
{
    template <class Iterator1, class Iterator2, class Function>
    class ZipIterator : public IteratorAdapter<ZipIterator<Iterator1, Iterator2, Function>>
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

        static_assert(std::is_same<difference_type1, difference_type2>::value, "Iterators must have the same difference_type.");

    private:
        Iterator1 _iterator1;
        Iterator2 _iterator2;
		Function _function;

    public:
        using iterator_category = std::common_type_t<iterator_category1, iterator_category2>;
        using value_type = decltype(_function(*_iterator1, *_iterator2));
        using difference_type = difference_type1;
        using reference = value_type;
		using pointer = DummyPointer<value_type>;

    public:
        ZipIterator(Iterator1 iterator1, Iterator2 iterator2, Function function)
			: _iterator1(iterator1), _iterator2(iterator2), _function(function)
        { }

        ZipIterator() = default;
        ZipIterator(ZipIterator const&) = default;
        ZipIterator(ZipIterator&&) = default;
        ZipIterator& operator=(ZipIterator const&) = default;
        ZipIterator& operator=(ZipIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(ZipIterator const& other) const { return _iterator1 == other._iterator1 && _iterator2 == other._iterator2; }
        reference Get() { return _function(*_iterator1, *_iterator2); }
        reference Get() const { return _function(*_iterator1, *_iterator2); }
		pointer operator->() { return CreateDummyPointer(Get()); }
		pointer operator->() const { return CreateDummyPointer(Get()); }
        void Increment() { ++_iterator1; ++_iterator2; }
        void Decrement() { --_iterator1; --_iterator2; }
        difference_type Difference(ZipIterator const& other) const { return _iterator1 - other._iterator1; }
        void Move(difference_type n) { _iterator1 += n; _iterator2 += n; }
    };

    template <class Iterator1, class Iterator2, class Function>
    auto CreateZipIterator(Iterator1 iterator1, Iterator2 iterator2, Function function)
    {
        return ZipIterator<Iterator1, Iterator2, Function>(iterator1, iterator2, function);
    }
}
