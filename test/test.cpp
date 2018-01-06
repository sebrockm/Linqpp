#include <iostream>
#include <vector>
#include <list>

#include "Linqpp.hpp"

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

void test()
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

    if (From(v).ElementAt(3) != 4)
        throw 8;

    if (From(l).ElementAt(2) != 8)
        throw 9;

    if (From(v).ElementAtOrDefault(-1) != 0)
        throw 10;

    if (From(l).ElementAtOrDefault(10) != 0)
        throw 11;

    if (From(v).ElementAtOrDefault(3) != 4)
        throw 12;

    if (From(l).ElementAtOrDefault(2) != 8)
        throw 13;

    auto rep = Enumerable::Repeat(1.234, 10);
    if (rep.Count() != 10 || !rep.All([](auto d) { return d == 1.234; }))
        throw 14;

    if (From(v).SequenceEqual(l))
        throw 15;

    if (!Enumerable::Range(1, 5).SequenceEqual(From(v)))
        throw 16;

    if (From(v).SkipWhile([](auto i) { return i < 4; }).Count() != 2)
        throw 17;

    if (From(v).SkipWhile([](auto i, auto j) { return j < 4; }).First() != 5)
        throw 18;

    if (Enumerable::Empty<std::vector<int>>().Any())
        throw 19;

    if (Enumerable::Empty<int>().DefaultIfEmpty(20).Count() != 1)
        throw 20;

    if (From(v).DefaultIfEmpty().Count() != v.size())
        throw 21;

    if (Enumerable::Empty<int>().DefaultIfEmpty(20).First() != 20)
        throw 22;

    std::vector<int> vy = testYield().ToVector();
    if (!testYield().SequenceEqual(vy))
        throw 23;

    if (!From(v).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(v).Count())))
        throw 24;

    if (!From(v).Select([](auto vi, auto i) { return vi; }).SequenceEqual(v))
        throw 25;

    if (!From(v).Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(From(v).Where([](auto vi) { return vi % 2 == 0; })))
        throw 26;

    if (!From(v).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(v).Take(3)))
        throw 27;

    if (From(v).Sum() != 15)
        throw 28;

    if (From(v).Sum([](auto i) { return i * i; }) != 55)
        throw 29;

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
        std::cout << i << " ";
    std::cout << std::endl;

    for (auto i : From(l).Reverse())
        std::cout << i << " ";
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
    std::cout << From(l).Aggregate(std::plus<>()) << std::endl;
    std::cout << Enumerable::Range(1, 10).Aggregate(std::string(""), [](auto s, auto j) { return s + ", " + std::to_string(j); }) << std::endl;

    std::vector<int> u = { 1, 1, 2, 3, 3, 1, 2, 3, 4 };
    std::cout << From(u).Distinct(std::less<>()).Count() << std::endl;
    std::cout << From(u).Distinct().Count() << std::endl;

    std::list<int> w = { 5, -1, 4, 3, 1 };
    std:: cout << From(w).Union(u).Count() << std::endl;

    struct A
    {
        int a;
        bool operator<(A a) const { return this->a < a.a; }
    };

    std::vector<A> va = { A{1}, A{3}, A{3}, A{1} };
    std::cout << From(va).Distinct().Count() << std::endl;

    struct B
    {
        int b;
        bool operator==(B b) const { return this->b == b.b; }
    };

    std::vector<B> vb = { B{2}, B{4}, B{1}, B{2}, B{1} };
    std::cout << From(vb).Distinct().Count() << std::endl;

    for (auto b : From(vb).OrderBy([](auto const& b) -> auto const& { return b.b; }))
        std::cout << b.b << ", ";
    std::cout << std::endl;

    for (auto b : From(vb).OrderByDescending([](auto const& b) -> auto const& { return b.b; }))
        std::cout << b.b << ", ";
    std::cout << std::endl;

    for (auto b : From(vb).OrderBy([](auto const& b) -> auto const& { return b; }, [](auto const& b1, auto const& b2) { return b1.b < b2.b; }))
        std::cout << b.b << ", ";
    std::cout << std::endl;

    for (auto b : From(vb).OrderByDescending([](auto const& b) -> auto const& { return b; }, [](auto const& b1, auto const& b2) { return b1.b < b2.b; }))
        std::cout << b.b << ", ";
    std::cout << std::endl;
}

int main()
{
    try
    {
        test();
    }
    catch (int i)
    {
        std::cerr << "test " << i << " failed." << std::endl;
        return 1;
    }

    std::cout << "All tests passed" << std::endl;
    return 0;
}
