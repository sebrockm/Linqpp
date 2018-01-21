#pragma once

#include <algorithm>
#include <iterator>
#include <numeric>

#include "ConcatIterator.hpp"
#include "Distinct.hpp"
#include "ElementAt.hpp"
#include "Enumerable.hpp"
#include "ExtendingEnumeration.hpp"
#include "From.hpp"
#include "Last.hpp"
#include "MinMax.hpp"
#include "SelectIterator.hpp"
#include "Skip.hpp"
#include "SortingIterator.hpp"
#include "TakeIterator.hpp"
#include "WhereIterator.hpp"
#include "ZipIterator.hpp"

namespace Linqpp
{
    template <class InputIterator>
    auto From(InputIterator first, InputIterator last);

    template <class InputIterator>
    class EnumerationBase
    {
    public:
        using value_type = typename std::iterator_traits<InputIterator>::value_type;
        using iterator_category = typename std::iterator_traits<InputIterator>::iterator_category;
        using iterator = InputIterator;

    protected:
        EnumerationBase() = default;

    public:
        virtual ~EnumerationBase() = default;

    public:
        virtual InputIterator begin() const = 0;
        virtual InputIterator end() const = 0;

    public:
        template <class BinaryFunction>
        auto Aggregate(BinaryFunction&& binaryFunction) const
        {
            decltype(auto) first = First();
            return std::accumulate(std::next(begin()), end(), first, std::forward<BinaryFunction>(binaryFunction));
        }

        template <class Seed, class BinaryFunction>
        auto Aggregate(Seed const& seed, BinaryFunction&& binaryFunction) const
        {
            return std::accumulate(begin(), end(), seed, std::forward<BinaryFunction>(binaryFunction));
        }

        template <class Seed, class BinaryFunction, class ResultSelector>
        auto Aggregate(Seed const& seed, BinaryFunction&& binaryFunction, ResultSelector&& resultSelector) const
        {
            return std::forward<ResultSelector>(resultSelector)(Aggregate(seed, std::forward<BinaryFunction>(binaryFunction)));
        }

        bool Any() const { return begin() != end(); }

        template <class Predicate> 
        bool Any(Predicate&& predicate) const { return std::any_of(begin(), end(), std::forward<Predicate>(predicate)); }

        template <class Predicate> 
        bool All(Predicate&& predicate) const { return std::all_of(begin(), end(), std::forward<Predicate>(predicate)); }

        auto Average() const
        {
            auto avg = First() * 1.0;
            size_t n = 1;
            for (auto const& i : Skip(1))
            {
                avg *= (double)n;
                ++n;
                avg /= (double)n;
                avg += i / (double)n;
            }

            return avg;
        }

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

        auto DefaultIfEmpty() const { return DefaultIfEmpty(value_type{}); }

        auto DefaultIfEmpty(value_type const& value) const { return Concat(Enumerable::Repeat(value, Any() ? 0 : 1)); }

        auto Distinct() const { return Linqpp::Distinct(begin(), end()); }

        template <class LessThanComparer>
        auto Distinct(LessThanComparer&& comparer) const { return Linqpp::Distinct(begin(), end(), std::forward<LessThanComparer>(comparer)); }

        template <class EqualityComparer, class Hash>
        auto Distinct(EqualityComparer&& comparer, Hash&& hash) const
        {
            return Linqpp::Distinct(begin(), end(), std::forward<EqualityComparer>(comparer), std::forward<Hash>(hash));
        }
        
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
        auto Select(UnaryFunction&& unaryFunction) const { return InternalSelect(std::forward<UnaryFunction>(unaryFunction), nullptr); }

        template <class Container>
        auto SequenceEqual(Container&& container) const
        {
            return std::equal(begin(), end(), std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)));
        }

        template <class Container, class EqualityComparer>
        auto SequenceEqual(Container&& container, EqualityComparer&& comparer) const
        {
            return std::equal(begin(), end(),
                    std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)),
                    std::forward<EqualityComparer>(comparer));
        }

        auto Skip(size_t n) const { return From(Linqpp::Skip(begin(), n, end()), end()); }

        template <class Predicate> 
        auto SkipWhile(Predicate&& predicate) const { return From(Linqpp::SkipWhile(begin(), end(), std::forward<Predicate>(predicate)), end()); }

        auto Sum() const { return Aggregate(std::plus<>()); }

        template <class UnaryFunction>
        auto Sum(UnaryFunction&& unaryFunction) const { return Select(std::forward<UnaryFunction>(unaryFunction)).Sum(); }

        auto Take(size_t n) const { return GetEnumeratorFromTake(begin(), n, end()); }

        auto ToVector() const { return ExtendingEnumeration<std::vector<value_type>>(begin(), end()); }

        template <class Container>
        auto Union(Container&& container) const { return Concat(std::forward<Container>(container)).Distinct(); }

        template <class Container, class LessThanComparer>
        auto Union(Container&& container, LessThanComparer&& comparer) const
        {
            return Concat(std::forward<Container>(container)).Distinct(std::forward<LessThanComparer>(comparer));
        }

        template <class Container, class EqualityComparer, class Hash>
        auto Union(Container&& container, EqualityComparer&& comparer, Hash&& hash) const
        {
            return Concat(std::forward<Container>(container)).Distinct(std::forward<EqualityComparer>(comparer), std::forward<Hash>(hash));
        }

        template <class Predicate>
        auto Where(Predicate&& predicate) const { return InternalWhere(std::forward<Predicate>(predicate), nullptr); }

        template <class Container, class BinaryFunction>
        auto Zip(Container&& container, BinaryFunction binaryFunction) const
        {
            auto cBegin = std::begin(std::forward<Container>(container));
            auto cEnd = std::end(std::forward<Container>(container));

            return From(CreateZipIterator(begin(), cBegin, binaryFunction), CreateZipIterator(end(), cEnd, binaryFunction));
        }

    private:
        template <class UnaryFunction>
        auto InternalSelect(UnaryFunction unaryFunction, decltype(std::declval<UnaryFunction>()(std::declval<value_type>()))*) const
        {
            return From(CreateSelectIterator(begin(), unaryFunction), CreateSelectIterator(end(), unaryFunction));
        }

        template <class UnaryFunctionWithIndex> 
        auto InternalSelect(UnaryFunctionWithIndex&& unaryFunctionWithIndex, decltype(std::declval<UnaryFunctionWithIndex>()(std::declval<value_type>(), 0))*) const
        {
            return Zip(Enumerable::Range(0, Count()), std::forward<UnaryFunctionWithIndex>(unaryFunctionWithIndex));
        }

        template <class Predicate> 
        auto InternalWhere(Predicate predicate, decltype(std::declval<Predicate>()(std::declval<value_type>()))*) const
        {
            auto first = CreateWhereIterator(begin(), end(), predicate);
            auto last = CreateWhereIterator(end(), end(), predicate);
            return From(first, last);
        }

        template <class PredicateWithIndex> 
        auto InternalWhere(PredicateWithIndex predicateWithIndex, decltype(std::declval<PredicateWithIndex>()(std::declval<value_type>(), 0))*) const
        {
            return Zip(Enumerable::Range(0, Count()), [](auto const& v, auto i) { return std::pair<decltype(v), decltype(i)>(v, i); })
                .Where([=](auto const& p) { return predicateWithIndex(p.first, p.second); }).Select([](auto const& p) { return p.first; });
        }
    };
}
