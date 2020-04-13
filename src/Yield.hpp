#pragma once

#include <condition_variable>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "IEnumerable.hpp"
#include "iterator/YieldingIterator.hpp"

namespace Linqpp
{
    namespace Yielding
    {
        template <class T>
        class ThreadController
        {
            enum class ThreadStatus
            {
                Uninitialized,
                ThreadIsWorking,
                CallerIsWorking,
                ThreadFinished
            };

        private:
            std::thread _yieldingThread;
            std::mutex _mutex;
            std::condition_variable _cv;
            std::unique_ptr<T> _spCurrentValue;
            std::exception_ptr _spException;
            ThreadStatus _threadStatus = ThreadStatus::Uninitialized;

        public:
            ThreadController() = default;

            ~ThreadController()
            {
                if (_yieldingThread.joinable())
                    _yieldingThread.join();
            }

            ThreadController(ThreadController const&) = delete;
            ThreadController(ThreadController&&) = delete;
            ThreadController& operator=(ThreadController const&) = delete;
            ThreadController& operator=(ThreadController&&) = delete;

        // Methods to be called from main thread only
        public:
            template <class YieldingFunction>
            void Initialize(YieldingFunction yieldingFunction, std::weak_ptr<ThreadController> spThreadController)
            {
                std::unique_lock<std::mutex> lock(_mutex);

                if (_threadStatus != ThreadStatus::Uninitialized)
                    throw std::logic_error("Attempt to assign a new yielding thread to already initialized thread controller.");

                _threadStatus = ThreadStatus::ThreadIsWorking;
                _yieldingThread = std::thread(std::move(yieldingFunction), std::move(spThreadController));

                while (_threadStatus == ThreadStatus::ThreadIsWorking)
                    _cv.wait(lock);
            }

            bool IsInitialized()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                return _threadStatus != ThreadStatus::Uninitialized;
            }

            T AwaitValueFromThread()
            {
                std::unique_lock<std::mutex> lock(_mutex);

                if (_threadStatus == ThreadStatus::Uninitialized)
                    throw std::runtime_error("Thread controller has not been initialized yet.");

                while (_threadStatus == ThreadStatus::ThreadIsWorking)
                    _cv.wait(lock);

                return std::move(*_spCurrentValue);
            }

            void ContinueYieldingThread()
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);

                    if (_threadStatus == ThreadStatus::Uninitialized)
                        throw std::runtime_error("Thread controller has not been initialized yet.");

                    if (_threadStatus == ThreadStatus::ThreadFinished)
                        return;

                    _threadStatus = ThreadStatus::ThreadIsWorking;
                }
                _cv.notify_one();

                {
                    std::unique_lock<std::mutex> lock(_mutex);

                    while (_threadStatus == ThreadStatus::ThreadIsWorking)
                        _cv.wait(lock);

                    if (_spException)
                        std::rethrow_exception(_spException);
                }
            }

            bool IsFinished()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                return _threadStatus == ThreadStatus::ThreadFinished;
            }

        // Methods to be called from yielding thread only
        public:
            template <class _T>
            void PassValueToCaller(_T&& t)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _spCurrentValue = std::make_unique<T>(std::forward<_T>(t));
                    _threadStatus = ThreadStatus::CallerIsWorking;
                }
                _cv.notify_all();
                WaitForCaller();
            }

            void PassExceptionToCaller(std::exception_ptr spException)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _spException = std::move(spException);
                    _threadStatus = ThreadStatus::CallerIsWorking;
                }
                _cv.notify_all();
                WaitForCaller();
            }

            void PassThreadFinishedNotificationToCaller()
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _threadStatus = ThreadStatus::ThreadFinished;
                }
                _cv.notify_all();
            }

        private:
            void WaitForCaller()
            {
                std::unique_lock<std::mutex> lock(_mutex);

                while (_threadStatus == ThreadStatus::CallerIsWorking)
                    _cv.wait(lock);
            }
        };

        template <class T>
        class YieldingEnumerable : public IEnumerable<YieldingIterator<T>>
        {
        private:
            YieldingIterator<T> _first;
            YieldingIterator<T> _last;

        public:
            YieldingEnumerable(std::function<void(std::weak_ptr<Yielding::ThreadController<T>>)> yieldingFunction)
                : _first(std::move(yieldingFunction)), _last()
            { }

            YieldingEnumerable(YieldingEnumerable const&) = default;
            YieldingEnumerable(YieldingEnumerable&&) = default;
            YieldingEnumerable& operator=(YieldingEnumerable const&) = default;
            YieldingEnumerable& operator=(YieldingEnumerable&&) = default;

        public:
            virtual YieldingIterator<T> begin() const override { return _first; }
            virtual YieldingIterator<T> end() const override { return _last; }
        };
    }
}

#define START_YIELDING(__type) \
    using __Type = __type; \
    return Linqpp::Yielding::YieldingEnumerable<__Type>([=](std::weak_ptr<Linqpp::Yielding::ThreadController<__Type>> __spThreadController) mutable \
    { \
        try \
        {

#define yield_return(__value) \
            { \
                auto __spThreadController2 = __spThreadController.lock(); \
                if (__spThreadController2) \
                    __spThreadController2->PassValueToCaller(__value); \
            }

#define END_YIELDING \
        } \
        catch (...) \
        { \
            auto __spThreadController2 = __spThreadController.lock(); \
            if (__spThreadController2) \
                __spThreadController2->PassExceptionToCaller(std::current_exception()); \
        } \
        auto __spThreadController2 = __spThreadController.lock(); \
        if (__spThreadController2) \
            __spThreadController2->PassThreadFinishedNotificationToCaller(); \
    });
