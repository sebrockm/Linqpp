#pragma once

#include "Enumerable.hpp"
#include "Utility.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class EnumerationBase;

    template <class Container>
    class ExtendingEnumeration : public Container, public EnumerationBase<Utility::Iterator<Container>>
    {
    public:
        using Container::Container;

    public:
        virtual Utility::Iterator<Container> begin() const override { return Container::begin(); }
        virtual Utility::Iterator<Container> end() const override { return Container::end(); }
    };
}
