#pragma once

#include "IteratorEnumerable.hpp"
#include "Utility.hpp"

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last)
    {
        return IteratorEnumerable<InputIterator>(first, last);
    }

    template <class Container, class = std::enable_if_t<!std::is_convertible<std::remove_reference_t<Container>*, IEnumerable<Utility::Iterator<Container>>*>::value>>
    auto From(Container&& container)
    {
        return From(std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)));
    }

    template <class Container, class = std::enable_if_t<std::is_convertible<std::remove_reference_t<Container>*, IEnumerable<Utility::Iterator<Container>>*>::value>>
    auto&& From(Container&& container)
    {
        return std::forward<Container>(container);
    }
}
