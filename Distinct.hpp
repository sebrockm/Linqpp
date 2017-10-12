#include "From.hpp"
#include "Enumeration.hpp"

#include <algorithm>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>

namespace Linqpp
{
    template <class InputIterator, class LessThanComparer>
    auto Distinct(InputIterator first, InputIterator last, LessThanComparer&& comparer)
    {
        using Value = typename std::iterator_traits<InputIterator>::value_type;

        auto distinguisher =
            [spSet = std::make_shared<std::set<Value, LessThanComparer>>(std::forward<LessThanComparer>(comparer))]
            (auto const& item)
        {
            return spSet->insert(item).second;
        };

        return From(first, last).Where(std::move(distinguisher));
    }

    template <class InputIterator, class EqualityComparer, class Hash>
    auto Distinct(InputIterator first, InputIterator last, EqualityComparer&& comparer, Hash&& hash)
    {
        using Value = typename std::iterator_traits<InputIterator>::value_type;

        auto distinguisher = 
            [spSet = std::make_shared<std::unordered_set<Value, Hash, EqualityComparer>>(1024, std::forward<Hash>(hash), std::forward<EqualityComparer>(comparer))]
            (auto const& item)
        {
            return spSet->insert(item).second;
        };

        return From(first, last).Where(std::move(distinguisher));
    }

    namespace Detail
    {
        // has no less
        template <class InputIterator>
        auto InternalDistinct2(InputIterator first, InputIterator last, ...)
        {
            using Value = typename std::iterator_traits<InputIterator>::value_type;

            auto distinguisher =
                [spSet = std::make_shared<std::vector<Value>>()]
                (auto const& item)
            {
                if (std::find(spSet->begin(), spSet->end(), item) != spSet->end())
                    return false;

                spSet->push_back(item);
                return true;
            };

            return From(first, last).Where(std::move(distinguisher));
        }

        // has less
        template <class InputIterator, class Value = typename std::iterator_traits<InputIterator>::value_type, class = decltype(std::declval<Value>() < std::declval<Value>())>
        auto InternalDistinct2(InputIterator first, InputIterator last, int)
        {
            return Distinct(first, last, std::less<>());
        }

        // has hash
        template <class InputIterator, class Value = typename std::iterator_traits<InputIterator>::value_type,
             class = decltype(std::hash<Value>()(std::declval<Value>())), class = decltype(std::declval<Value>() == std::declval<Value>())>
        auto InternalDistinct1(InputIterator first, InputIterator last, int)
        {
            return Distinct(first, last, std::equal_to<>(), std::hash<Value>());
        }
        
        // has no hash
        template <class InputIterator>
        auto InternalDistinct1(InputIterator first, InputIterator last, ...)
        {
            return InternalDistinct2(first, last, 0);
        }
    }

    template <class InputIterator>
    auto Distinct(InputIterator first, InputIterator last)
    {
        return Detail::InternalDistinct1(first, last, 0);
    }
}
