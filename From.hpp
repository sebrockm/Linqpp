#pragma once

#include "IteratorEnumeration.hpp"

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last)
    {
        return IteratorEnumeration<InputIterator>(first, last);
    }

    template <class Container>
    auto From(Container&& container)
    {
        return From(std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)));
    }
}
