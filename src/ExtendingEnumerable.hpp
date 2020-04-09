#pragma once

#include "Enumerable.hpp"
#include "Utility.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class IEnumerable;

    template <class Container>
    class ExtendingEnumerable : public Container, public IEnumerable<Utility::Iterator<Container>>
    {
    public:
        using Container::Container;

    public:
        virtual Utility::Iterator<Container> begin() const override { return Container::begin(); }
        virtual Utility::Iterator<Container> end() const override { return Container::end(); }
    };
}
