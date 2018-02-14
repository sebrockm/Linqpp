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
            ThreadController() = default;

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

            T const& GetValue() const { return _current; }
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
                return YieldingIterator<T>(std::move(spThread), std::move(spThreadController));
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
    return Linqpp::Yielding::YieldingEnumerable<__Type>([=](std::shared_ptr<Linqpp::Yielding::ThreadController<__Type>> __spThreadController) \
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
                return; \

#define yield_return(__value) \
            { \
                __spThreadController->GetValue() = std::move(__value); \
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
