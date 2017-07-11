#include "Enumeration.hpp"
#include "Yield.hpp"

#include <iostream>

using namespace Linqpp;

auto testYield()
{
	auto spYielder = Yielder<int>::Create();
	spYielder->Start([=]()
	{
		spYielder->Yield(-17);
		for (int i = 0; i < 17; ++i)
			spYielder->Yield(i);
	});
	return spYielder->Return();
}

int main()
{
    std::vector<int> v = {1,2,3,4,5};

    if (!From(v).Any())
        throw 1;

    if (!From(v).Any([](auto i) { return i == 4; }))
        throw 2;

    if (From(v).All([](auto i) { return i < 4; }))
        throw 3;

    if (From(v).Skip(3).Take(1).Count() != 1)
        throw 4;

    std::list<int> l = {6,7,8,9};
    int j = 1;
    for (auto i : From(v).Concat(l))
        if (i != j++)
            throw 5;

	for (auto i : testYield().Concat(v))
		std::cout << i << " ";
    std::cout << std::endl;

    for (auto i : testYield().Take(100))
        std::cout << i << " ";
    std::cout << std::endl;

    for (auto i : From(v).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }))
        std::cout << i << " ";
    std::cout << std::endl;

    std::cout << "All tests passed" << std::endl;
}
