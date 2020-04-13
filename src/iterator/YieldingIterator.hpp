#pragma once

#include <limits>
#include <memory>

#include "IteratorAdapter.hpp"
#include "DummyPointer.hpp"

namespace Linqpp
{
    namespace Yielding
    {
        template <class T>
        class ThreadController;
    }

    template <class T>
    class YieldingIterator : public IteratorAdapter<YieldingIterator<T>>
    {
        static_assert(!std::is_reference<T>::value, "Yielding funtions cannot return reference types.");

    private:
        mutable std::shared_ptr<Yielding::ThreadController<T>> _spThreadController;
        std::function<void(std::weak_ptr<Yielding::ThreadController<T>>)> _yieldingFunction;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = value_type;
        using pointer = DummyPointer<value_type>;

    public:
        YieldingIterator(std::function<void(std::weak_ptr<Yielding::ThreadController<T>>)> yieldingFunction)
            : _spThreadController(std::make_shared<Yielding::ThreadController<T>>()), _yieldingFunction(std::move(yieldingFunction))
        { }

        YieldingIterator() = default;
        YieldingIterator(YieldingIterator<T> const&) = default;
        YieldingIterator(YieldingIterator<T>&&) = default;
        YieldingIterator& operator=(YieldingIterator<T> const&) = default;
        YieldingIterator& operator=(YieldingIterator<T>&&) = default;

    public:
        bool Equals(YieldingIterator<T> const& other) const
        {
            CheckInitialized();
            other.CheckInitialized();
            return _spThreadController == other._spThreadController;
        }

        reference Get() const
        {
            CheckInitialized();
            return _spThreadController->AwaitValueFromThread();
        }

        pointer operator->() const { return CreateDummyPointer(Get()); }

        void Increment()
        {
            CheckInitialized();

            if (!_spThreadController)
                return;
            
            _spThreadController->ContinueYieldingThread();

            if (_spThreadController->IsFinished())
                MarkAsEndIterator();
        }

    private:
        void CheckInitialized() const
        {
            if (_spThreadController && !_spThreadController->IsInitialized())
            {
                _spThreadController->Initialize(std::move(_yieldingFunction), _spThreadController);
                if (_spThreadController->IsFinished())
                    MarkAsEndIterator();
            }
        }

        void MarkAsEndIterator() const { _spThreadController = nullptr; }
    };
}
