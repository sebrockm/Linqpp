#pragma once

#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <limits>
#include <thread>

#include "IEnumerable.hpp"
#include "Iterator/YieldingIterator.hpp"
#include "Utility.hpp"

namespace Linqpp
{
    namespace Yielding
    {
        template <class T>
        class YieldStorage
        {
        private:
            T _value;

        public:
            T& Get() { return _value; }
            
            template <class _T>
            void Set(_T&& value) { _value = std::forward<_T>(value); }
        };

        template <class T>
        class YieldStorage<T&>
        {
        private:
            T* _pValue;

        public:
            T& Get() { return *_pValue; }

            template <class _T>
            void Set(_T&& value) { _pValue = &value; }
        };

        template <class T>
        class ThreadController
        {
        private:
            std::thread _thread;
            std::mutex _mutex;
            std::condition_variable _cv;
            bool _threadIsExecuting = false;
            bool _finished = false;
            std::exception_ptr _spException;
            YieldStorage<T> _current;

        public:
            ThreadController() = default;

            ~ThreadController()
            {
                if (_thread.joinable())
                    _thread.detach();

                WaitForThread();
                if (!IsThreadFinished())
                {
                    FinishThread();
                    SwitchToThread();
                }
            }

            ThreadController(ThreadController const&) = delete;
            ThreadController(ThreadController&&) = delete;
            ThreadController& operator=(ThreadController const&) = delete;
            ThreadController& operator=(ThreadController&&) = delete;

        public:
            void SetThread(std::thread thread) { _thread = std::move(thread); }

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
                std::unique_lock<std::mutex> lock(_mutex);
                _threadIsExecuting = false;
                _cv.notify_all();
            }

            void SwitchToThread()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _threadIsExecuting = true;
                _cv.notify_one();
            }

            T const& GetValue() const { return _current.Get(); }
            T& GetValue() { return _current.Get(); }

            template <class _T>
            void SetValue(_T&& t) { _current.Set(std::forward<_T>(t)); }

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
        class YieldingEnumerable : public IEnumerable<YieldingIterator<T>>
        {
        private:
            std::function<void(std::shared_ptr<ThreadController<T>>)> _function;

        public:
            YieldingEnumerable(std::function<void(std::shared_ptr<ThreadController<T>>)> function)
                : _function(std::move(function))
            { }

            YieldingEnumerable(YieldingEnumerable const&) = default;
            YieldingEnumerable(YieldingEnumerable&&) = default;
            YieldingEnumerable& operator=(YieldingEnumerable const&) = default;
            YieldingEnumerable& operator=(YieldingEnumerable&&) = default;

        public:
            virtual YieldingIterator<T> begin() const override
            {
                static_assert(std::is_copy_assignable<YieldingIterator<T>>::value, "YieldingIterator is not copy assignable.");
                return YieldingIterator<T>(_function);
            }

            virtual YieldingIterator<T> end() const override
            {
                return YieldingIterator<T>();
            }
        };
    }
}

#define START_YIELDING(__type) \
    using __Type = __type; \
    return Linqpp::Yielding::YieldingEnumerable<__Type>([&](std::shared_ptr<Linqpp::Yielding::ThreadController<__Type>> __spThreadController) mutable \
    { \
        auto __onExit = Linqpp::Utility::on_exit([=] \
        { \
            __spThreadController->FinishThread(); \
            __spThreadController->SwitchToHost(); \
        }); \
        try \
        { \
            __spThreadController->WaitForHost(); \
            if (__spThreadController->IsThreadFinished()) \
                return;

#define yield_return(__value) \
            { \
                __spThreadController->SetValue(__value); \
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
