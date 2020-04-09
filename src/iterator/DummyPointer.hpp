#pragma once

namespace Linqpp
{
    namespace Detail
    {
        template <class T>
        class DummyPointer
        {
        private:
            T _value;

        public:
            explicit DummyPointer(T&& value) : _value(std::move(value)) { }

            auto operator->() const { return std::addressof(_value); }
            operator T*() const { return std::addressof(_value); }
        };

        template <class T, class = std::enable_if_t<std::is_rvalue_reference<T&&>::value>>
        auto CreateDummyPointer(T&& value) { return DummyPointer<std::remove_reference_t<T>>(std::move(value)); }
    }
}
