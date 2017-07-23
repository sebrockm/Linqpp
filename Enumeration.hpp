#pragma once

#include "ConcatIterator.hpp"
#include "Last.hpp"
#include "MinMax.hpp"
#include "SelectIterator.hpp"
#include "Skip.hpp"
#include "TakeIterator.hpp"
#include "WhereIterator.hpp"
#include "ZipIterator.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <vector>

namespace Linqpp
{
    template <class Iterator>
    class Enumeration;

    template <class Iterator>
    auto From(Iterator first, Iterator last)
    {
        return Enumeration<Iterator>(first, last);
    }

    template <class Container>
    auto From(Container&& container)
    {
        return From(std::begin(std::forward<Container>(container)), std::end(std::forward<Container>(container)));
    }

    template <class Iterator>
    class Enumeration
    {
    private:
        Iterator _first;
        Iterator _last;

    public:
        using value_type = typename std::iterator_traits<Iterator>::value_type;

    public:
        Enumeration(Iterator first, Iterator last) : _first(first), _last(last) { }

    public:
        auto begin() const { return _first; }
        auto end() const { return _last; }

    public:
        template <class Seed, class BinaryFunction>
        auto Aggregate(Seed const& seed, BinaryFunction binaryFunction) const { return std::accumulate(_first, _last, seed, binaryFunction); }

        template <class BinaryFunction>
        auto Aggregate(BinaryFunction binaryFunction) const { return Aggregate(First(), binaryFunction); }

        bool Any() const { return _first != _last; }

        template <class Predicate> 
        bool Any(Predicate predicate) const { return std::any_of(_first, _last, predicate); }

        template <class Predicate> 
        bool All(Predicate predicate) const { return std::all_of(_first, _last, predicate); }

        template <class Container>
        auto Concat(Container&& container) const
        {
            auto cBegin = std::begin(std::forward<Container>(container));
            auto cEnd = std::end(std::forward<Container>(container));
            auto first = CreateConcatIterator(_first, _last, cBegin, cBegin);
            auto last = CreateConcatIterator(_last, _last, cBegin, cEnd);

            return From(first, last);
        }

        template <class T, class CompareFunction>
        auto Contains(T const& t, CompareFunction compareFunction) const { return Any([=](const auto& t2) { return compareFunction(t, t2); }); }

        template <class T>
        auto Contains(T const& t) const { return Contains(t, std::equal_to<>()); }

        size_t Count() const { return std::distance(_first, _last); }

        template <class Predicate>
        size_t Count(Predicate predicate) const { return std::count_if(_first, _last, predicate); }

        decltype(auto) First() const { return *_first; }

        template <class Predicate>
        decltype(auto) First(Predicate predicate) const { return *std::find_if(_first, _last, predicate); }

        value_type FirstOrDefault() const { return Any() ? First() : value_type(); }

        template <class Predicate>
        value_type FirstOrDefault(Predicate predicate) const { auto found = std::find_if(_first, _last, predicate); return found != _last ? *found : value_type(); }

        decltype(auto) Last() const { return *Linqpp::Last(_first, _last); }

        template <class Predicate>
        decltype(auto) Last(Predicate predicate) const { return *Linqpp::Last(_first, _last, predicate); }

        value_type LastOrDefault() const { return Any() ? Last() : value_type(); }

        template <class Predicate>
        value_type LastOrDefault(Predicate predicate) const { auto found = Linqpp::Last(_first, _last, predicate); return found != _last ? *found : value_type(); }

        decltype(auto) Max() const { return Linqpp::Max(_first, _last); }

        template <class UnaryFunction>
        decltype(auto) Max(UnaryFunction unaryFunction) const { return Select(unaryFunction).Max(); }

        decltype(auto) Min() const { return Linqpp::Min(_first, _last); }

        template <class UnaryFunction>
        decltype(auto) Min(UnaryFunction unaryFunction) const { return Select(unaryFunction).Min(); }

        template <class UnaryFunction>
        auto Select(UnaryFunction unaryFunction) const { return From(CreateSelectIterator(_first, unaryFunction), CreateSelectIterator(_last, unaryFunction)); }

        auto Skip(size_t n) const { return From(Linqpp::Skip(_first, n, _last), _last); }

        auto Take(size_t n) const { return GetEnumeratorFromTake(_first, n, _last); }

        auto ToVector() const { return std::vector<value_type>(_first, _last); }

        template <class Predicate>
        auto Where(Predicate predicate) const
        {
            auto first = CreateWhereIterator(_first, _last, predicate);
            auto last = CreateWhereIterator(_last, _last, predicate);
            return From(first, last);
        }

        template <class Container, class BinaryFunction>
        auto Zip(Container&& container, BinaryFunction binaryFunction) const
        {
            auto cBegin = std::begin(std::forward<Container>(container));
            auto cEnd = std::end(std::forward<Container>(container));

            return From(CreateZipIterator(_first, cBegin, binaryFunction), CreateZipIterator(_last, cEnd, binaryFunction));
        }
    };
}
