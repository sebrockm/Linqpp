#pragma once

#include "ConcatIterator.hpp"
#include "Distinct.hpp"
#include "ElementAt.hpp"
#include "From.hpp"
#include "IntIterator.hpp"
#include "Last.hpp"
#include "MinMax.hpp"
#include "SelectIterator.hpp"
#include "Skip.hpp"
#include "SortingIterator.hpp"
#include "TakeIterator.hpp"
#include "WhereIterator.hpp"
#include "ZipIterator.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <numeric>
#include <set>
#include <unordered_set>
#include <vector>

namespace Linqpp
{
    template <class InputIterator>
    class Enumeration
    {
    private:
        const InputIterator _first;
        const InputIterator _last;

    public:
        using value_type = typename std::iterator_traits<InputIterator>::value_type;
        using iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;

    public:
        Enumeration(InputIterator first, InputIterator last) : _first(first), _last(last) { }

    public:
        auto begin() const { return _first; }
        auto end() const { return _last; }

    public:
        template <class Seed, class BinaryFunction>
        auto Aggregate(Seed const& seed, BinaryFunction&& binaryFunction) const { return std::accumulate(begin(), end(), seed, std::forward<BinaryFunction>(binaryFunction)); }

        template <class BinaryFunction>
        auto Aggregate(BinaryFunction&& binaryFunction) const
        {
            decltype(auto) first = First();
            return std::accumulate(std::next(begin()), end(), first, std::forward<BinaryFunction>(binaryFunction));
        }

        bool Any() const { return begin() != end(); }

        template <class Predicate> 
        bool Any(Predicate&& predicate) const { return std::any_of(begin(), end(), std::forward<Predicate>(predicate)); }

        template <class Predicate> 
        bool All(Predicate&& predicate) const { return std::all_of(begin(), end(), std::forward<Predicate>(predicate)); }

        template <class Container>
        auto Concat(Container&& container) const
        {
            auto cBegin = std::begin(std::forward<Container>(container));
            auto cEnd = std::end(std::forward<Container>(container));
            auto first = CreateConcatIterator(begin(), end(), cBegin, cBegin);
            auto last = CreateConcatIterator(end(), end(), cBegin, cEnd);

            return From(first, last);
        }

        template <class T, class EqualityComparer>
        auto Contains(T const& t, EqualityComparer&& comparer) const { return Any([&](const auto& t2) { return std::forward<EqualityComparer>(comparer)(t, t2); }); }

        template <class T>
        auto Contains(T const& t) const { return Contains(t, std::equal_to<>()); }

        size_t Count() const { return std::distance(begin(), end()); }

        template <class Predicate>
        size_t Count(Predicate&& predicate) const { return std::count_if(begin(), end(), std::forward<Predicate>(predicate)); }

        auto Distinct() const { return Linqpp::Distinct(begin(), end()); }

        template <class LessThanComparer>
        auto Distinct(LessThanComparer&& comparer) const { return Linqpp::Distinct(begin(), end(), std::forward<LessThanComparer>(comparer)); }

        template <class EqualityComparer, class Hash>
        auto Distinct(EqualityComparer&& comparer, Hash&& hash) const { return Linqpp::Distinct(begin(), end(), std::forward<EqualityComparer>(comparer), std::forward<Hash>(hash)); }
        
        decltype(auto) ElementAt(int64_t i) const { return Linqpp::ElementAt(begin(), i, iterator_category()); }

        value_type ElementAtOrDefault(int64_t i) const { return Linqpp::ElementAtOrDefault(begin(), end(), i, iterator_category()); }

        decltype(auto) First() const { return *begin(); }

        template <class Predicate>
        decltype(auto) First(Predicate&& predicate) const { return *std::find_if(begin(), end(), std::forward<Predicate>(predicate)); }

        value_type FirstOrDefault() const { return Any() ? First() : value_type(); }

        template <class Predicate>
        value_type FirstOrDefault(Predicate&& predicate) const 
        {
            auto found = std::find_if(begin(), end(), std::forward<Predicate>(predicate));
            return found != end() ? *found : value_type();
        }

        decltype(auto) Last() const { return *Linqpp::Last(begin(), end()); }

        template <class Predicate>
        decltype(auto) Last(Predicate&& predicate) const { return *Linqpp::Last(begin(), end(), std::forward<Predicate>(predicate)); }

        value_type LastOrDefault() const { return Any() ? Last() : value_type(); }

        template <class Predicate>
        value_type LastOrDefault(Predicate&& predicate) const
        {
            auto found = Linqpp::Last(begin(), end(), std::forward<Predicate>(predicate));
            return found != end() ? *found : value_type();
        }

        decltype(auto) Max() const { return Linqpp::Max(begin(), end()); }

        template <class UnaryFunction>
        decltype(auto) Max(UnaryFunction&& unaryFunction) const { return Select(std::forward<UnaryFunction>(unaryFunction)).Max(); }

        decltype(auto) Min() const { return Linqpp::Min(begin(), end()); }

        template <class UnaryFunction>
        decltype(auto) Min(UnaryFunction&& unaryFunction) const { return Select(std::forward<UnaryFunction>(unaryFunction)).Min(); }

        template <class UnaryFunction>
        auto OrderBy(UnaryFunction&& unaryFunction) const { return OrderBy(std::forward<UnaryFunction>(unaryFunction), std::less<>()); }

        template <class UnaryFunction, class LessThanComparer>
        auto OrderBy(UnaryFunction unaryFunction, LessThanComparer comparer) const 
        { 
            return CreateSortedEnumeration(begin(), end(), [=] (auto const& t1, auto const& t2) { return comparer(unaryFunction(t1), unaryFunction(t2)); });
        }

        template <class UnaryFunction>
        auto OrderByDescending(UnaryFunction&& unaryFunction) const { return OrderBy(std::forward<UnaryFunction>(unaryFunction), std::greater<>()); }

        template <class UnaryFunction, class LessThanComparer>
        auto OrderByDescending(UnaryFunction&& unaryFunction, LessThanComparer comparer) const
        {
            return OrderBy(std::forward<UnaryFunction>(unaryFunction), [=] (auto const& t1, auto const& t2) { return comparer(t2, t1); });
        }

        auto Reverse() const { return From(std::make_reverse_iterator(end()), std::make_reverse_iterator(begin())); }

        template <class UnaryFunction>
        auto Select(UnaryFunction unaryFunction) const { return From(CreateSelectIterator(begin(), unaryFunction), CreateSelectIterator(end(), unaryFunction)); }

        template <class Container>
        auto SequenceEqual(Container&& container) const
        {
            return std::equal(begin(), end(), std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)));
        }

        template <class Container, class EqualityComparer>
        auto SequenceEqual(Container&& container, EqualityComparer&& comparer) const
        {
            return std::equal(begin(), end(), std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)), std::forward<EqualityComparer>(comparer));
        }

        auto Skip(size_t n) const { return From(Linqpp::Skip(begin(), n, end()), end()); }

        auto Take(size_t n) const { return GetEnumeratorFromTake(begin(), n, end()); }

        auto ToVector() const { return std::vector<value_type>(begin(), end()); }

        template <class Predicate>
        auto Where(Predicate predicate) const
        {
            auto first = CreateWhereIterator(begin(), end(), predicate);
            auto last = CreateWhereIterator(end(), end(), predicate);
            return From(first, last);
        }

        template <class Container, class BinaryFunction>
        auto Zip(Container&& container, BinaryFunction binaryFunction) const
        {
            auto cBegin = std::begin(std::forward<Container>(container));
            auto cEnd = std::end(std::forward<Container>(container));

            return From(CreateZipIterator(begin(), cBegin, binaryFunction), CreateZipIterator(end(), cEnd, binaryFunction));
        }
    };

    struct Enumerable
    {
        template <class Int>
        static auto Range(Int start, Int count)
        {
            return From(CreateIntIterator(start), CreateIntIterator(start + count));
        }

        template <class T>
        static auto Repeat(T t, size_t n)
        {
            return Range((size_t)0, n).Select([=](auto const&) { return t; });
        }
    };
}
