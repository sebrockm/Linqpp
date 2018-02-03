#pragma once

#include <limits>
#include <memory>
#include <thread>

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
        std::shared_ptr<std::thread> _spThread;
        std::shared_ptr<Yielding::ThreadController<T>> _spThreadController;
        size_t _position;

    public:
        using iterator_category = std::input_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using reference = std::add_lvalue_reference_t<T>;
        using pointer = std::add_pointer_t<T>;

    public:
        YieldingIterator(std::shared_ptr<std::thread> spThread, std::shared_ptr<Yielding::ThreadController<T>> spThreadController)
            : _spThread(std::move(spThread)), _spThreadController(std::move(spThreadController)), _position(0)
        {
            Increment();
        }

        YieldingIterator()
            : _position(std::numeric_limits<size_t>::max())
        { }

        YieldingIterator(YieldingIterator<T> const&) = default;
        YieldingIterator(YieldingIterator<T>&&) = default;
        YieldingIterator& operator=(YieldingIterator<T> const&) = default;
        YieldingIterator& operator=(YieldingIterator<T>&&) = default;

    public:
        bool Equals(YieldingIterator<T> const& other) const { return _spThread == other._spThread && _position == other._position; }

        reference Get() const { return _spThreadController->GetValue(); }

        void Increment()
        {
            if (_position == std::numeric_limits<size_t>::max())
                return;

            _spThreadController->SwitchToThread();
            _spThreadController->WaitForThread();
            _spThreadController->TryRethrow();

            if (!_spThreadController->IsThreadFinished())
                ++_position;
            else
            {
                _position = std::numeric_limits<size_t>::max();
                _spThreadController = nullptr;
                _spThread = nullptr;
            }
        }
    };
}
