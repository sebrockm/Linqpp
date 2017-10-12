#pragma once

#include "Enumeration.hpp"
#include "From.hpp"
#include "IntIterator.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class Enumeration;

    struct Enumerable
    {
        template <class Int>
        static auto Range(Int start, Int count)
        {
            return From(CreateIntIterator(start), CreateIntIterator(start + count));
        }

        template <class T>
        static auto Repeat(T t, size_t n)
        {
            return Range((size_t)0, n).Select([=](auto const&) { return t; });
        }
    };
}
