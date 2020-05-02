#pragma once

#include <algorithm>

#include "IteratorAdapter.hpp"
#include "DummyPointer.hpp"

using namespace Linqpp::Detail;

namespace Linqpp
{
    template <class InputIterator1, class InputIterator2, class BinaryFunction>
    class ZipIterator : public IteratorAdapter<ZipIterator<InputIterator1, InputIterator2, BinaryFunction>>
    {
        using difference_type1 = typename std::iterator_traits<InputIterator1>::difference_type;
        using difference_type2 = typename std::iterator_traits<InputIterator2>::difference_type;
        using iterator_category1 = typename std::iterator_traits<InputIterator1>::iterator_category;
        using iterator_category2 = typename std::iterator_traits<InputIterator2>::iterator_category;

        static_assert(std::is_same<difference_type1, difference_type2>::value, "Iterators must have the same difference_type.");

    private:
        InputIterator1 _iterator1;
        InputIterator2 _iterator2;
        BinaryFunction _function;

    public:
        using iterator_category = std::common_type_t<iterator_category1, iterator_category2>;
        using value_type = decltype(_function(*_iterator1, *_iterator2));
        using difference_type = difference_type1;
        using reference = value_type;
        using pointer = DummyPointer<value_type>;

    public:
        ZipIterator(InputIterator1 iterator1, InputIterator2 iterator2, BinaryFunction function)
            : _iterator1(iterator1), _iterator2(iterator2), _function(std::move(function))
        { }

        ZipIterator() = default;
        ZipIterator(ZipIterator const&) = default;
        ZipIterator(ZipIterator&&) = default;

        ZipIterator& operator=(ZipIterator other)
        {
            Swap(*this, other);
            return *this;
        }

    // IteratorAdapter
    public:
        bool Equals(ZipIterator const& other) const { return _iterator1 == other._iterator1 || _iterator2 == other._iterator2; }
        reference Get() const { return _function(*_iterator1, *_iterator2); }
        pointer operator->() const { return CreateDummyPointer(Get()); }
        void Increment() { ++_iterator1; ++_iterator2; }
        void Decrement() { --_iterator1; --_iterator2; }
        difference_type Difference(ZipIterator const& other) const { return _iterator1 - other._iterator1; }
        void Move(difference_type n) { _iterator1 += n; _iterator2 += n; }

    // Internals
    private:
        friend void swap(ZipIterator& iterator1, ZipIterator& iterator2)
        {
            return Swap(iterator1, iterator2, std::is_copy_assignable<BinaryFunction>());
        }

        static void Swap(ZipIterator& iterator1, ZipIterator& iterator2, std::true_type)
        {
            using std::swap;
            Swap(iterator1, iterator2, std::false_type());
            swap(iterator1._function, iterator2._function);
        }

        static void Swap(ZipIterator& iterator1, ZipIterator& iterator2, std::false_type)
        {
            using std::swap;
            swap(iterator1._iterator1, iterator2._iterator1);
            swap(iterator1._iterator2, iterator2._iterator2);
        }
    };

    template <class InputIterator1, class InputIterator2, class BinaryFunction>
    auto CreateZipIterator(InputIterator1 iterator1, InputIterator2 iterator2, BinaryFunction function)
    {
        static_assert(std::is_copy_assignable<ZipIterator<InputIterator1, InputIterator2, BinaryFunction>>::value, "ZipIterator is not copy assignable.");

        return ZipIterator<InputIterator1, InputIterator2, BinaryFunction>(iterator1, iterator2, function);
    }

    template <class InputIterator>
    auto From(InputIterator first, InputIterator last);

    template <class RandomIterator1, class RandomIterator2, class BinaryFunction>
    auto CreateZipEnumerable(RandomIterator1 first1, RandomIterator1 last1, std::random_access_iterator_tag,
            RandomIterator2 first2, RandomIterator2 last2, std::random_access_iterator_tag, BinaryFunction function)
    {
        auto zipFirst = CreateZipIterator(first1, first2, function);
        return From(zipFirst, zipFirst + std::min(last1 - first1, last2 - first2));
    }

    template <class InputIterator1, class InputIterator2, class BinaryFunction>
    auto CreateZipEnumerable(InputIterator1 first1, InputIterator1 last1, std::input_iterator_tag,
            InputIterator2 first2, InputIterator2 last2, std::input_iterator_tag, BinaryFunction function)
    {
        auto zipFirst = CreateZipIterator(first1, first2, function);
        auto zipLast = CreateZipIterator(last1, last2, function);
        return From(zipFirst, zipLast);
    }
}
