#pragma once

#include "EnumerationBase.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class IteratorEnumeration : public EnumerationBase<InputIterator>
    {
    private:
        const InputIterator _first;
        const InputIterator _last;

    public:
        IteratorEnumeration(InputIterator first, InputIterator last) : _first(first), _last(last) { }

    public:
        virtual InputIterator begin() const override { return _first; }
        virtual InputIterator end() const override { return _last; }
    };
}
