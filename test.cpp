#include "Enumeration.hpp"

#include <iostream>

using namespace Linqpp;

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

    std::cout << "All tests passed" << std::endl;
}
