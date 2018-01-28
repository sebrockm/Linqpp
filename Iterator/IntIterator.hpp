#pragma once

#include "IteratorAdapter.hpp"

namespace Linqpp
{
    template <class Int>
    class IntIterator : public IteratorAdapter<IntIterator<Int>>
    {
    // Fields
    private:
        Int _int;
        static_assert(std::is_integral<Int>::value, "No valid integral type");
        static_assert(!std::is_same<Int, bool>::value, "Bool is not allowed");

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::make_signed_t<Int>;
        using value_type = Int;
        using reference = Int;
        using pointer = Int*;

    // Constructors, destructor
    public:
        IntIterator(Int integer)
            : _int(integer)
        { }

        IntIterator() = default;
        IntIterator(IntIterator const&) = default;
        IntIterator(IntIterator&&) = default;
        IntIterator& operator=(IntIterator const&) = default;
        IntIterator& operator=(IntIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(IntIterator const& other) const { return _int == other._int; }
        reference Get() const { return _int; }
        void Increment() { ++_int; }
        void Decrement() { --_int; }
        difference_type Difference(IntIterator const& other) const { return _int - other._int; }
        void Move(difference_type n) { _int += n; }
    };

    template <class Int>
    auto CreateIntIterator(Int integer)
    {
        static_assert(std::is_copy_assignable<IntIterator<Int>>::value, "IntIterator is not copy assignable.");
        static_assert(std::is_move_assignable<IntIterator<Int>>::value, "IntIterator is not move assignable.");
        return IntIterator<Int>(integer);
    }
}
