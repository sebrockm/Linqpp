#pragma once

#include "Enumerable.hpp"

namespace Linqpp
{
    template <class Iterator>
    class Enumeration;

    template <class Iterator>
    auto From(Iterator first, Iterator last)
    {
        return Enumeration<Iterator>(first, last);
    }

    template <class Container>
    auto From(Container&& container)
    {
        return From(std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)));
    }
}
