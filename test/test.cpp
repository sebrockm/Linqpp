#include "Enumeration.hpp"
#include "Yield.hpp"

#include <iostream>
#include <vector>
#include <list>

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

    if (From(v).Skip(6).Any())
        throw 5;

    if (From(v).Take(10).Count() != v.size())
        throw 6;

    std::list<int> l = {6,7,8,9};
    int j = 1;
    for (auto i : From(v).Concat(l))
        if (i != j++)
            throw 7;

    for (auto i : testYield().Concat(v))
        std::cout << i << " ";
    std::cout << std::endl;

    for (auto i : testYield().Take(100))
        std::cout << i << " ";
    std::cout << std::endl;

    for (auto i : From(v).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }))
        std::cout << i << " ";
    std::cout << std::endl;

    for (auto i : From(v).Where([](auto i) { return i % 2 == 0; }))
        std:: cout << i << " ";
    std::cout << std::endl;

    auto s = From(v).Take(4).Zip(l, [](auto i, auto j) { return std::make_pair(i, j); });

    for (auto it = s.begin(); it != s.end(); ++it)
        std::cout << it->first << "," << it->second << "\n";

    std::cout << From(v).First() << " " << From(v).Skip(23).FirstOrDefault() << std::endl;
    std::cout << From(v).Last() << " " << From(v).Skip(23).LastOrDefault() << std::endl;
    //std::cout << testYield().Last() << std::endl;
    std::cout << From(l).Contains(7) << std::endl;
    std::cout << From(l).Min() << " " << From(l).Max() << std::endl;
    std::cout << From(l).Min(std::negate<>()) << " " << From(l).Max(std::negate<>()) << std::endl;

    std::cout << "All tests passed" << std::endl;
}
