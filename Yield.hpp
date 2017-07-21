#pragma once

#include "Enumeration.hpp"
#include "IteratorAdapter.hpp"

#include <memory>
#include <future>
#include <condition_variable>
#include <queue>

namespace Linqpp
{
    template <class T, size_t MaxBufferSize = 100>
    class Yielder : public std::enable_shared_from_this<Yielder<T, MaxBufferSize>>
    {
    private:
        std::future<void> _generator;
        std::condition_variable _cv;
        std::mutex _mutex;
        std::queue<T> _buffer;
        bool _startedGenerating = false;
        bool _finishedGenerating = false;
        bool _destructorCalled = false;

    private:
        Yielder() = default;
        Yielder(Yielder const&) = delete;
        Yielder& operator=(Yielder const&) = delete;

        class Iterator : public IteratorAdapter<Iterator>
        {
        private:
            std::shared_ptr<Yielder> _spYielder;
            bool _isEnd;

        public:
            using iterator_category = std::input_iterator_tag;
            using value_type = T;
            using difference_type = std::ptrdiff_t;
            using reference = std::add_lvalue_reference_t<T>;
            using pointer = std::add_pointer_t<T>;

        public:
            Iterator() : _isEnd(true) { };
            Iterator(std::shared_ptr<Yielder> const& spYielder) : _spYielder(spYielder), _isEnd(false) { Increment(); }
            Iterator(Iterator const&) = default;
            Iterator(Iterator&&) = default;
            Iterator& operator=(Iterator const&) = default;
            Iterator& operator=(Iterator&&) = default;

        public:
            bool Equals(Iterator const& other) const { return _isEnd == other._isEnd; }

            reference Get() const
            {
                std::unique_lock<std::mutex> lock(_spYielder->_mutex);
                return _spYielder->_buffer.front();
            }

            void Increment()
            {
                {
                    std::unique_lock<std::mutex> lock(_spYielder->_mutex);
                    if (!_spYielder->_buffer.empty()) // should be empty only in the very first call of Advance
                        _spYielder->_buffer.pop();
                    _spYielder->_cv.wait(lock, [=]() { return _spYielder->_finishedGenerating || !_spYielder->_buffer.empty(); });

                    if (_spYielder->_buffer.empty())
                        _isEnd = true;
                }
                _spYielder->_cv.notify_all();
            }
        };

        friend class Iterator;

        class ThreadEndException { };

    public:
        ~Yielder()
        {
            _destructorCalled = true;
            _cv.notify_all();
            _generator.wait();
        }

        template <class _T>
        void Yield(_T&& t)
        {
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _cv.wait(lock, [=]() { return _buffer.size() < MaxBufferSize || _destructorCalled; });

                if (_destructorCalled)
                    throw ThreadEndException();

                _buffer.push(std::forward<_T>(t));
            }
            _cv.notify_all();
        }

        template <class Generator>
        void Start(Generator generator)
        {
            _generator = std::async(std::launch::async, [=]()
            {
                try
                {
                    generator();
                }
                catch (ThreadEndException) { }
                _finishedGenerating = true;
                _cv.notify_all();
            });
            _startedGenerating = true;
        }

        auto Return()
        {
            while (!_startedGenerating);

            return From(Iterator(this->shared_from_this()), Iterator());
        }

    public:
        static auto Create() { return std::shared_ptr<Yielder>(new Yielder()); }
    };
}
