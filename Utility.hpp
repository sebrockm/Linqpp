#pragma once

namespace Linqpp
{
    namespace Utility
    {
        template <class Container>
        using Iterator = decltype(std::begin(std::declval<Container>()));
    }
}
