#pragma once

#include <limits>
#include <memory>

#include "IteratorAdapter.hpp"

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
    private:
        std::shared_ptr<Yielding::ThreadController<T>> _spThreadController;
        size_t _position;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = std::add_lvalue_reference_t<T>;
        using pointer = std::add_pointer_t<T>;

    public:
        YieldingIterator(std::function<void(std::shared_ptr<Yielding::ThreadController<T>>)> const& function)
            : _spThreadController(std::make_shared<Yielding::ThreadController<T>>()), _position(0)
        {
            _spThreadController->SetThread(std::thread(function, _spThreadController));
            Increment();
        }

        YieldingIterator()
        {
            MarkAsEnd();
        }

        YieldingIterator(YieldingIterator<T> const&) = default;
        YieldingIterator(YieldingIterator<T>&&) = default;
        YieldingIterator& operator=(YieldingIterator<T> const&) = default;
        YieldingIterator& operator=(YieldingIterator<T>&&) = default;

    public:
        bool Equals(YieldingIterator<T> const& other) const { return _spThreadController == other._spThreadController && _position == other._position; }

        reference Get() const { return _spThreadController->GetValue(); }

        void Increment()
        {
            if (_spThreadController && _spThreadController->IsThreadFinished())
                MarkAsEnd();

            if (_position == std::numeric_limits<size_t>::max())
                return;

            _spThreadController->SwitchToThread();
            _spThreadController->WaitForThread();
            _spThreadController->TryRethrow();

            if (!_spThreadController->IsThreadFinished())
                ++_position;
            else
                MarkAsEnd();
        }

    private:
        void MarkAsEnd()
        {
            _position = std::numeric_limits<size_t>::max();
            _spThreadController = nullptr;
        }
    };
}
