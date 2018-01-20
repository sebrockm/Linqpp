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
        struct ThreadData
        {
            std::mutex _mutex;
            std::condition_variable _cv;
            bool _threadIsExecuting = false;
            bool _finished = false;
            T _current;
            std::exception_ptr _spException;
        };

        template <class T>
        class YieldingIterator : public IteratorAdapter<YieldingIterator<T>>
        {
        private:
            std::shared_ptr<std::thread> _spThread;
            std::shared_ptr<ThreadData<T>> _spThreadData;
            size_t _position;
            mutable bool _isIncrementDeferred = false;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using reference = std::add_lvalue_reference_t<T>;
            using pointer = std::add_pointer_t<T>;

        public:
            YieldingIterator(std::shared_ptr<std::thread> spThread, std::shared_ptr<ThreadData<T>> spThreadData)
                : _spThread(std::move(spThread)), _spThreadData(std::move(spThreadData)), _position(0)
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
            bool Equals(YieldingIterator<T> const& other) const
            {
                CatchUp();
                other.CatchUp();
                return _spThread == other._spThread && _position == other._position;
            }

            reference Get() const
            {
                CatchUp();
                return _spThreadData->_current;
            }

            void Increment()
            {
                if (_position == std::numeric_limits<size_t>::max())
                    return;

                if (!_isIncrementDeferred)
                {
                    _isIncrementDeferred = true;
                    return;
                }

                {
                    std::unique_lock<std::mutex> lock(_spThreadData->_mutex);
                    _spThreadData->_threadIsExecuting = true;
                }
                _spThreadData->_cv.notify_one();
                bool finished = false;
                {
                    std::unique_lock<std::mutex> lock(_spThreadData->_mutex);
                    while (_spThreadData->_threadIsExecuting)
                        _spThreadData->_cv.wait(lock);

                    if (_spThreadData->_spException)
                        std::rethrow_exception(_spThreadData->_spException);

                    if (_spThreadData->_finished)
                        finished = true;
                    else
                        ++_position;
                }

                if (finished)
                {
                    _position = std::numeric_limits<size_t>::max();
                    _spThreadData = nullptr;
                    _spThread = nullptr;
                }
            }

        private:
            void CatchUp() const
            {
                if (_isIncrementDeferred)
                {
                    const_cast<YieldingIterator<T>*>(this)->Increment();
                    _isIncrementDeferred = false;
                }
            }
        };

        template <class T>
        class YieldingEnumeration : public EnumerationBase<YieldingIterator<T>>
        {
        private:
            std::function<void(std::shared_ptr<ThreadData<T>>)> _function;

        public:
            YieldingEnumeration(std::function<void(std::shared_ptr<ThreadData<T>>)> function)
                : _function(std::move(function))
            { }

        public:
            virtual YieldingIterator<T> begin() const override
            {
                auto spThreadData = std::make_shared<ThreadData<T>>();
                auto spThread = std::shared_ptr<std::thread>(new std::thread(_function, spThreadData), [=](std::thread* pThread)
                {
                    if (pThread->joinable())
                        pThread->detach();
                    delete pThread;

                    std::unique_lock<std::mutex> lock(spThreadData->_mutex);
                    while (spThreadData->_threadIsExecuting)
                        spThreadData->_cv.wait(lock);
                    
                    if (!spThreadData->_finished)
                    {
                        spThreadData->_threadIsExecuting = true;
                        spThreadData->_finished = true;
                        spThreadData->_cv.notify_one();
                    }
                });
                return YieldingIterator<T>(std::move(spThread), std::move(spThreadData));
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
    return Linqpp::Yielding::YieldingEnumeration<__Type>([=](std::shared_ptr<Linqpp::Yielding::ThreadData<__Type>> __spThreadData) \
    { \
        auto __onExit = Linqpp::Yielding::on_exit([=] \
        { \
            { \
                std::unique_lock<std::mutex> __lock(__spThreadData->_mutex); \
                __spThreadData->_threadIsExecuting = false; \
                __spThreadData->_finished = true; \
            } \
            __spThreadData->_cv.notify_all(); \
        }); \
        try \
        { \
            { \
                std::unique_lock<std::mutex> __lock(__spThreadData->_mutex); \
                while (!__spThreadData->_threadIsExecuting) \
                    __spThreadData->_cv.wait(__lock); \
                if (__spThreadData->_finished) \
                    return; \
            }

#define yield_return(__value) \
            { \
                { \
                    std::unique_lock<std::mutex> __lock(__spThreadData->_mutex); \
                    __spThreadData->_current = (__value); \
                    __spThreadData->_threadIsExecuting = false; \
                } \
                __spThreadData->_cv.notify_all(); \
                { \
                    std::unique_lock<std::mutex> __lock(__spThreadData->_mutex); \
                    while (!__spThreadData->_threadIsExecuting) \
                        __spThreadData->_cv.wait(__lock); \
                    if (__spThreadData->_finished) \
                        return; \
                } \
            }

#define END_YIELDING \
        } \
        catch (...) \
        { \
            std::unique_lock<std::mutex> __lock(__spThreadData->_mutex); \
            __spThreadData->_spException = std::current_exception(); \
        } \
    });
