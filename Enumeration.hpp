#pragma once

#include "ConcatIterator.hpp"

#include <iterator>
#include <algorithm>
#include <vector>
#include <list>

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
		Enumeration(Iterator first, Iterator last)
			: _first(first), _last(last)
		{ }

	public:
		auto begin() const { return _first; }
		auto end() const { return _last; }

	public:
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

		size_t Count() const { return std::distance(_first, _last); }

		auto Skip(size_t n) const { return From(std::next(_first, n), _last); }

		auto Take(size_t n) const { return From(_first, std::next(_first, n)); }

		auto ToVector() const { return std::vector<value_type>(_first, _last); }
	};
}
