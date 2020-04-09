#pragma once

#include "IEnumerable.hpp"

namespace Linqpp
{
    template <class InputIterator>
    class IteratorEnumerable : public IEnumerable<InputIterator>
    {
    private:
        InputIterator _first;
        InputIterator _last;

    public:
        IteratorEnumerable(InputIterator first, InputIterator last) : _first(first), _last(last) { }

        IteratorEnumerable() = default;
        IteratorEnumerable(IteratorEnumerable const&) = default;
        IteratorEnumerable(IteratorEnumerable&&) = default;
        IteratorEnumerable& operator=(IteratorEnumerable const&) = default;
        IteratorEnumerable& operator=(IteratorEnumerable&&) = default;

    public:
        virtual InputIterator begin() const override { return _first; }
        virtual InputIterator end() const override { return _last; }
    };
}
