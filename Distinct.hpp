#include <algorithm>
#include <set>
#include <unordered_set>
#include <vector>

#include "Enumerable.hpp"

namespace Linqpp
{
    template <class Iterator>
    class Enumeration;

    template <class Iterator>
    auto From(Iterator, Iterator);

    template <class Iterator, class LessThanComparer>
    auto Distinct(Iterator first, Iterator last, LessThanComparer comparer)
    {
        using Value = typename std::iterator_traits<Iterator>::value_type;

        const auto distinguisher =
            [spSet = std::make_shared<std::set<Value, LessThanComparer>>(comparer)]
            (auto const& item)
        {
            return spSet->insert(item).second;
        };

        return From(first, last).Where(distinguisher);
    }

    template <class Iterator, class EqualityComparer, class Hash>
    auto Distinct(Iterator first, Iterator last, EqualityComparer comparer, Hash hash)
    {
        using Value = typename std::iterator_traits<Iterator>::value_type;

        const auto distinguisher = 
            [spSet = std::make_shared<std::unordered_set<Value, Hash, EqualityComparer>>(1024, hash, comparer)]
            (auto const& item)
        {
            return spSet->insert(item).second;
        };

        return From(first, last).Where(distinguisher);
    }

    namespace Detail
    {
        // has no less
        template <class Iterator>
        auto InternalDistinct2(Iterator first, Iterator last, ...)
        {
            using Value = typename std::iterator_traits<Iterator>::value_type;

            const auto distinguisher =
                [spSet = std::make_shared<std::vector<Value>>()]
                (auto const& item)
            {
                if (std::find(spSet->begin(), spSet->end(), item) != spSet->end())
                    return false;

                spSet->push_back(item);
                return true;
            };

            return From(first, last).Where(distinguisher);
        }

        // has less
        template <class Iterator, class Value = typename std::iterator_traits<Iterator>::value_type, class = decltype(std::declval<Value>() < std::declval<Value>())>
        auto InternalDistinct2(Iterator first, Iterator last, int)
        {
            return Distinct(first, last, std::less<>());
        }

        // has hash
        template <class Iterator, class Value = typename std::iterator_traits<Iterator>::value_type,
             class = decltype(std::hash<Value>()(std::declval<Value>())), class = decltype(std::declval<Value>() == std::declval<Value>())>
        auto InternalDistinct1(Iterator first, Iterator last, int)
        {
            return Distinct(first, last, std::equal_to<>(), std::hash<Value>());
        }
        
        // has no hash
        template <class Iterator>
        auto InternalDistinct1(Iterator first, Iterator last, ...)
        {
            return InternalDistinct2(first, last, 0);
        }
    }

    template <class Iterator>
    auto Distinct(Iterator first, Iterator last)
    {
        return Detail::InternalDistinct1(first, last, 0);
    }
}
