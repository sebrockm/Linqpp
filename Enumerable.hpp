#pragma once

namespace Linqpp
{
    struct Enumerable
    {
        template <class T>
        static auto Empty();

        template <class Int1, class Int2>
        static auto Range(Int1 start, Int2 count);

        template <class T>
        static auto Repeat(T t, size_t n);
    };
}

#include "From.hpp"
#include "Iterator/IntIterator.hpp"

#include <array>

namespace Linqpp
{
    template <class T>
    auto Enumerable::Empty()
    {
        return ExtendingEnumeration<std::array<T, 0>>();
    }

    template <class Int1, class Int2>
    auto Enumerable::Range(Int1 start, Int2 count)
    {
        return From(CreateIntIterator(start + (Int2)0), CreateIntIterator(start + count));
    }

    template <class T>
    auto Enumerable::Repeat(T t, size_t n)
    {
        return Range((size_t)0, n).Select([=](size_t) { return t; });
    }
}
