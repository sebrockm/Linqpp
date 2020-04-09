#pragma once

namespace Linqpp
{
    namespace Utility
    {
        template <class Container>
        using Iterator = decltype(std::begin(std::declval<Container>()));

        template <class Function>
        class OnExit
        {
        private:
            Function _function;

        public:
            OnExit(Function  function)
                : _function(function)
            { }

            ~OnExit() { _function(); }
        };

        template <class Function>
        auto on_exit(Function function) { return OnExit<Function>(function); }
    }
}
