#pragma once

#include "Enumerable.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class EnumerationBase;

    template <class Container>
    using Iterator = decltype(std::begin(std::declval<Container>()));

    template <class Container>
    class ExtendingEnumeration : public Container, public EnumerationBase<Iterator<Container>>
    {
    public:
        using Container::Container;

    public:
        virtual Iterator<Container> begin() const override { return Container::begin(); }
        virtual Iterator<Container> end() const override { return Container::end(); }
    };
}
