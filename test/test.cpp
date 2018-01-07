#include <iostream>
#include <vector>
#include <list>

#include "Linqpp.hpp"

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

using namespace Linqpp;


TEST_CASE("unit tests")
{
    struct A
    {
        int a;
        bool operator<(A a) const { return this->a < a.a; }
    };
    struct B
    {
        int b;
        bool operator==(B b) const { return this->b == b.b; }
    };

    std::vector<int> u = { 1, 1, 2, 3, 3, 1, 2, 3, 4 };
    std::vector<int> v = { 1,2,3,4,5 };
    std::list<int> l = { 6,7,8,9 };

    std::vector<A> va = { A{1}, A{3}, A{3}, A{1} };
    std::vector<B> vb = { B{2}, B{4}, B{1}, B{2}, B{1} };
    


    SECTION("Aggregate")
    {
        CHECK(From(v).Aggregate(std::plus<>()) == 15);
        CHECK(Enumerable::Range(1, 10).Aggregate(std::string(""), [](auto s, auto j) { return s + ", " + std::to_string(j); })
                == ", 1, 2, 3, 4, 5, 6, 7, 8, 9, 10");
    }

    SECTION("All")
    {
        CHECK_FALSE(From(v).All([](auto i) { return i < 4; }));
        CHECK(From(v).All([](auto i) { return i < 10; }));
    }

    SECTION("Any")
    {
        CHECK(From(v).Any());
        CHECK(From(v).Any([](auto i) { return i == 4; }));
    }

    SECTION("Concat")
    {
        int j = 1;
        for (auto i : From(v).Concat(l))
            CHECK(i == j++);
    }

    SECTION("Contains")
    {
        CHECK(From(v).Contains(4));
        CHECK_FALSE(From(v).Contains(7));
        CHECK(From(l).Contains(7));
        CHECK_FALSE(From(l).Contains(4));
    }

    SECTION("Distinct")
    {
        std::vector<int> d = { 1, 2, 3, 4 };

        CHECK(From(u).Distinct().SequenceEqual(d));
        CHECK(From(u).Distinct(std::less<>()).SequenceEqual(d));
        CHECK(From(u).Distinct(std::equal_to<>(), std::hash<int>()).SequenceEqual(d));
        CHECK(From(va).Distinct().SequenceEqual(std::vector<A>{{1}, {3}}, [](auto a1, auto a2) { return a1.a == a2.a; }));
        CHECK(From(vb).Distinct().SequenceEqual(std::vector<B>{{2}, {4}, {1}}));
    }

    SECTION("ElementAt")
    {
        CHECK(From(v).ElementAt(3) == 4);
        CHECK(From(l).ElementAt(2) == 8);
        CHECK(From(v).ElementAtOrDefault(-1) == 0);
        CHECK(From(l).ElementAtOrDefault(10) == 0);
        CHECK(From(v).ElementAtOrDefault(3) == 4);
        CHECK(From(l).ElementAtOrDefault(2) == 8);
    }

    SECTION("Empty")
    {
        CHECK_FALSE(Enumerable::Empty<std::vector<int>>().Any());
        CHECK(Enumerable::Empty<int>().DefaultIfEmpty(20).Count() == 1);
        CHECK(From(v).DefaultIfEmpty().Count() == v.size());
        CHECK(Enumerable::Empty<int>().DefaultIfEmpty(20).First() == 20);
    }

    SECTION("First")
    {
        CHECK(From(v).First() == 1);
        CHECK(From(v).Skip(23).FirstOrDefault() == 0);
    }

    SECTION("Last")
    {
        CHECK(From(v).Last() == 5);
        CHECK(From(v).Skip(23).LastOrDefault() == 0);
    }

    SECTION("MinMax")
    {
        CHECK(From(v).Min() == 1);
        CHECK(From(v).Max() == 5);
        CHECK(From(v).Min(std::negate<>()) == -5);
        CHECK(From(v).Max(std::negate<>()) == -1);
    }

    SECTION("OrderBy")
    {
        std::vector<B> vbs = { B{1}, B{1}, B{2}, B{2}, B{4} };
        std::vector<B> vbsd = { B{4}, B{2}, B{2}, B{1}, B{1} };

        CHECK(From(vb).OrderBy([](auto const& b) { return b.b; }).SequenceEqual(vbs));
        CHECK(From(vb).OrderByDescending([](auto const& b) { return b.b; }).SequenceEqual(vbsd));
        CHECK(From(vb).OrderBy([](auto const& b) { return b; }, [](auto const& b1, auto const& b2) { return b1.b < b2.b; }).SequenceEqual(vbs));
        CHECK(From(vb).OrderByDescending([](auto const& b) { return b; }, [](auto const& b1, auto const& b2) { return b1.b < b2.b; }).SequenceEqual(vbsd));
    }

    SECTION("Repeat")
    {
        auto rep = Enumerable::Repeat(1.234, 10);

        CHECK(rep.Count() == 10);
        CHECK(rep.All([](auto d) { return d == 1.234; }));
    }

    SECTION("Reverse")
    {
        CHECK(From(v).Reverse().SequenceEqual(std::vector<int>{5, 4, 3, 2, 1}));
    }

    SECTION("Select")
    {
        CHECK(From(v).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(v).Count())));
        CHECK(From(v).Select([](auto vi, auto i) { return vi; }).SequenceEqual(v));
        CHECK(From(v).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'1'", "'2'", "'3'", "'4'", "'5'"}));
    }

    SECTION("SequenceEqual")
    {
        CHECK_FALSE(From(v).SequenceEqual(l));
        CHECK(Enumerable::Range(1, 5).SequenceEqual(From(v)));
    }

    SECTION("Skip")
    {
        CHECK(From(v).Skip(3).Count() == v.size() - 3);
        CHECK(From(v).Skip(6).Count() == 0);
    }

    SECTION("SkipWhile")
    {
        CHECK(From(v).SkipWhile([](auto i) { return i < 4; }).Count() == 2);
        CHECK(From(v).SkipWhile([](auto i, auto j) { return j < 4; }).First() == 5);
    }

    SECTION("Sum")
    {
        CHECK(From(v).Sum() == 15);
        CHECK(From(v).Sum([](auto i) { return i * i; }) == 55);
    }

    SECTION("Take")
    {
        CHECK(From(v).Take(3).Count() == 3);
        CHECK(From(v).Take(10).Count() == v.size());
    }

    SECTION("Union")
    {
        std::list<int> w = { 5, -1, 4, 3, 1 };
        std::vector<int> d = { 5, -1, 4, 3, 1, 2 };

        CHECK(From(w).Union(u).SequenceEqual(d));
        CHECK(From(w).Union(u, std::less<>()).SequenceEqual(d));
        CHECK(From(w).Union(u, std::equal_to<>(), std::hash<int>()).SequenceEqual(d));
    }

    SECTION("Where")
    {
        CHECK(From(v).Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(From(v).Where([](auto vi) { return vi % 2 == 0; })));
        CHECK(From(v).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(v).Take(3)));
        CHECK(From(v).Where([](auto i) { return i % 2 == 0; }).SequenceEqual(std::vector<int>{2, 4}));
    }

    SECTION("Yield")
    {
        auto testYield = []()
        {
            auto spYielder = Yielder<int>::Create();
            spYielder->Start([=]()
            {
                spYielder->Yield(-17);
                for (int i = 0; i < 17; ++i)
                    spYielder->Yield(i);
            });
            return spYielder->Return();
        };

        CHECK(testYield().Count() == 18);
        CHECK(testYield().SequenceEqual(Enumerable::Repeat(-17, 1).Concat(Enumerable::Range(0, 17))));
        CHECK(testYield().SequenceEqual(testYield()));
        CHECK(testYield().Take(100).SequenceEqual(testYield()));
    }

    SECTION("Zip")
    {
        CHECK(From(v).Take(4).Zip(l, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 6}, {2, 7}, {3, 8}, {4, 9}}));
    }
}
