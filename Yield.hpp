#pragma once

#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <limits>
#include <thread>

#include "EnumerationBase.hpp"
#include "IteratorAdapter.hpp"

namespace Linqpp
{
    namespace Yielding
    {
        template <class T>
        class ThreadController
        {
        private:
            std::mutex _mutex;
            std::condition_variable _cv;
            bool _threadIsExecuting = false;
            bool _finished = false;
            T _current;
            std::exception_ptr _spException;

        public:
            void WaitForHost()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                while (!_threadIsExecuting)
                    _cv.wait(lock);
            }

            void WaitForThread()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                while (_threadIsExecuting)
                    _cv.wait(lock);
            }

            void SwitchToHost()
            {
                _threadIsExecuting = false;
                _cv.notify_all();
            }

            void SwitchToThread()
            {
                _threadIsExecuting = true;
                _cv.notify_one();
            }

            T& GetValue() { return _current; }

            void SetException(std::exception_ptr spException) { _spException = spException; }

            void TryRethrow() const
            {
                if (_spException)
                    std::rethrow_exception(_spException);
            }

            bool IsThreadFinished() const { return _finished; }
            void FinishThread() { _finished = true; }

        };

        template <class T>
        class YieldingIterator : public IteratorAdapter<YieldingIterator<T>>
        {
        private:
            std::shared_ptr<std::thread> _spThread;
            std::shared_ptr<ThreadController<T>> _spThreadController;
            size_t _position;
            mutable bool _isIncrementDeferred = false;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using reference = std::add_lvalue_reference_t<T>;
            using pointer = std::add_pointer_t<T>;

        public:
            YieldingIterator(std::shared_ptr<std::thread> spThread, std::shared_ptr<ThreadController<T>> spThreadController)
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

        template <class T>
        class YieldingEnumeration : public EnumerationBase<YieldingIterator<T>>
        {
        private:
            std::function<void(std::shared_ptr<ThreadController<T>>)> _function;

        public:
            YieldingEnumeration(std::function<void(std::shared_ptr<ThreadController<T>>)> function)
                : _function(std::move(function))
            { }

        public:
            virtual YieldingIterator<T> begin() const override
            {
                auto spThreadController = std::make_shared<ThreadController<T>>();
                auto spThread = std::shared_ptr<std::thread>(new std::thread(_function, spThreadController), [=](std::thread* pThread)
                {
                    if (pThread->joinable())
                        pThread->detach();
                    delete pThread;

                    spThreadController->WaitForThread();
                    if (!spThreadController->IsThreadFinished())
                    {
                        spThreadController->FinishThread();
                        spThreadController->SwitchToThread();
                    }
                });
                static_assert(std::is_copy_assignable<YieldingIterator<T>>::value, "YieldingIterator is not copy assignable.");
                static_assert(std::is_move_assignable<YieldingIterator<T>>::value, "YieldingIterator is not move assignable.");
                return YieldingIterator<T>(std::move(spThread), std::move(spThreadController));
            }

            virtual YieldingIterator<T> end() const override
            {
                return YieldingIterator<T>();
            }
        };

        template <class Function>
        class OnExit
        {
        private:
            Function _function;

        public:
            OnExit(Function const& function)
                : _function(function)
            { }

            ~OnExit() { _function(); }
        };

        template <class Function>
        auto on_exit(Function&& function) { return OnExit<Function>(std::forward<Function>(function)); }
    }
}

#define START_YIELDING(__type) \
    using __Type = __type; \
    return Linqpp::Yielding::YieldingEnumeration<__Type>([=](std::shared_ptr<Linqpp::Yielding::ThreadController<__Type>> __spThreadController) \
    { \
        auto __onExit = Linqpp::Yielding::on_exit([=] \
        { \
            __spThreadController->FinishThread(); \
            __spThreadController->SwitchToHost(); \
        }); \
        try \
        { \
            __spThreadController->WaitForHost(); \
            if (__spThreadController->IsThreadFinished()) \
                return; \

#define yield_return(__value) \
            { \
                __spThreadController->GetValue() = (__value); \
                __spThreadController->SwitchToHost(); \
                __spThreadController->WaitForHost(); \
                if (__spThreadController->IsThreadFinished()) \
                    return; \
            }

#define END_YIELDING \
        } \
        catch (...) \
        { \
            __spThreadController->SetException(std::current_exception()); \
        } \
    });
