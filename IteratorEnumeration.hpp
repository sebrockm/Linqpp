#pragma once

#include "EnumerationBase.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class IteratorEnumeration : public EnumerationBase<InputIterator>
    {
    private:
        InputIterator _first;
        InputIterator _last;

    public:
        IteratorEnumeration(InputIterator first, InputIterator last) : _first(first), _last(last) { }

        IteratorEnumeration() = default;
        IteratorEnumeration(IteratorEnumeration const&) = default;
        IteratorEnumeration(IteratorEnumeration&&) = default;
        IteratorEnumeration& operator=(IteratorEnumeration const&) = default;
        IteratorEnumeration& operator=(IteratorEnumeration&&) = default;

    public:
        virtual InputIterator begin() const override { return _first; }
        virtual InputIterator end() const override { return _last; }
    };
}
