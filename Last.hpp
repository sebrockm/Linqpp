#pragma once

#include <iterator>

namespace Linqpp
{
	namespace Detail
	{
		template <class Iterator>
		decltype(auto) InternalLast(Iterator first, Iterator last, std::bidirectional_iterator_tag) { return *--last; }

		template <class Iterator>
		decltype(auto) InternalLast(Iterator first, Iterator last, std::input_iterator_tag)
		{
			while (true)
			{
				auto element = *first;
				if (++first == last)
					return element;
			}
		}
	}

	template <class Iterator>
	decltype(auto) Last(Iterator first, Iterator last)
	{
		return Detail::InternalLast(first, last, typename std::iterator_traits<Iterator>::iterator_category());
	}
}
