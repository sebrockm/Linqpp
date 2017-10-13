#pragma once

namespace Linqpp
{
    struct Enumerable
    {
        template <class T>
        static auto Empty();

        template <class Int>
        static auto Range(Int start, Int count);

        template <class T>
        static auto Repeat(T t, size_t n);
    };
}

#include "From.hpp"
#include "IntIterator.hpp"

namespace Linqpp
{
    template <class T>
    auto Enumerable::Empty()
    {
        return Range(0, 0).Select([](auto const&) { return *(T*)nullptr; }).ToVector();
    }

    template <class Int>
    auto Enumerable::Range(Int start, Int count)
    {
        return From(CreateIntIterator(start), CreateIntIterator(start + count));
    }

    template <class T>
    auto Enumerable::Repeat(T t, size_t n)
    {
        return Range((size_t)0, n).Select([=](auto const&) { return t; });
    }
}
