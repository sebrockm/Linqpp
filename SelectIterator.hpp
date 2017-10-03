#pragma once

#include "IteratorAdapter.hpp"
#include "DummyPointer.hpp"

using namespace Linqpp::Detail;

namespace Linqpp
{
    template <class InputIterator, class UnaryFunction>
    class SelectIterator : public IteratorAdapter<SelectIterator<InputIterator, UnaryFunction>>
    {
    // Fields
    private:
        InputIterator _iterator;
        UnaryFunction _function;

    public:
        using iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;
        using difference_type = typename std::iterator_traits<InputIterator>::difference_type;
        using value_type = decltype(_function(*_iterator));
        using reference = value_type;
        using pointer = DummyPointer<value_type>;

    // Constructors, destructor
    public:
        SelectIterator(InputIterator iterator, UnaryFunction function)
            : _iterator(iterator), _function(std::move(function))
        { }

        SelectIterator() = default;
        SelectIterator(SelectIterator const&) = default;
        SelectIterator(SelectIterator&&) = default;
        SelectIterator& operator=(SelectIterator const&) = default;
        SelectIterator& operator=(SelectIterator&&) = default;

    // IteratorAdapter
    public:
        bool Equals(SelectIterator const& other) const { return _iterator == other._iterator; }
        reference Get() const { return _function(*_iterator); }
        pointer operator->() const { return CreateDummyPointer(Get()); }
        void Increment() { ++_iterator; }
        void Decrement() { --_iterator; }
        difference_type Difference(SelectIterator const& other) const { return _iterator - other._iterator; }
    };

    template <class InputIterator, class UnaryFunction>
    auto CreateSelectIterator(InputIterator iterator, UnaryFunction&& function)
    {
        return SelectIterator<InputIterator, std::remove_reference_t<UnaryFunction>>(iterator, std::forward<UnaryFunction>(function));
    }
}
