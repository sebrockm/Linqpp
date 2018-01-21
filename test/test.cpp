#include <iostream>
#include <vector>
#include <list>
#include <forward_list>

#include "Linqpp.hpp"
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

    auto testYield = []()
    {
        START_YIELDING(int)

        yield_return(-1);
        for (int i = 0; i < 7; ++i)
            yield_return(i);

        END_YIELDING
    };


    std::vector<int> ran = { 1, 2, 3, 4, 5 };
    std::list<int> bid = { 6, 7, 8, 9 };
    std::forward_list<int> forw = { 3, 4, 5, 6, 7 };
    auto inp = testYield();

    std::vector<int> u = { 1, 1, 2, 3, 3, 1, 2, 3, 4 };
    std::vector<A> va = { A{1}, A{3}, A{3}, A{1} };
    std::vector<B> vb = { B{2}, B{4}, B{1}, B{2}, B{1} };
    
    SECTION("Aggregate")
    {
        CHECK(From(ran).Aggregate(std::plus<>()) == 15);
        CHECK(From(bid).Aggregate(std::plus<>()) == 30);
        CHECK(From(forw).Aggregate(std::plus<>()) == 25);
        CHECK(From(inp).Aggregate(std::plus<>()) == 20);

        auto plus = [](std::string s, int i) { return s + std::to_string(i); };
        CHECK(From(ran).Aggregate(std::string(""), plus) == "12345");
        CHECK(From(bid).Aggregate(std::string(""), plus) == "6789");
        CHECK(From(forw).Aggregate(std::string(""), plus) == "34567");
        CHECK(From(inp).Aggregate(std::string(""), plus) == "-10123456");

        auto to_int = [](std::string s) { return std::stoi(s); };
        CHECK(From(ran).Aggregate(std::string(""), plus, to_int) == 12345);
        CHECK(From(bid).Aggregate(std::string(""), plus, to_int) == 6789);
        CHECK(From(forw).Aggregate(std::string(""), plus, to_int) == 34567);
        CHECK(From(inp).Aggregate(std::string(""), plus, to_int) == -10123456);
    }

    SECTION("All")
    {
        CHECK_FALSE(From(ran).All([](auto i) { return i < 4; }));
        CHECK(From(ran).All([](auto i) { return i < 10; }));
        CHECK_FALSE(From(bid).All([](auto i) { return i < 4; }));
        CHECK(From(bid).All([](auto i) { return i < 10; }));
        CHECK_FALSE(From(forw).All([](auto i) { return i < 4; }));
        CHECK(From(forw).All([](auto i) { return i < 10; }));
        CHECK_FALSE(From(inp).All([](auto i) { return i < 2; }));
        CHECK(From(inp).All([](auto i) { return i < 10; }));
    }

    SECTION("Any")
    {
        CHECK(From(ran).Any());
        CHECK(From(ran).Any([](auto i) { return i == 4; }));
        CHECK_FALSE(From(ran).Any([](auto i) { return i > 10; }));
        CHECK(From(bid).Any());
        CHECK(From(bid).Any([](auto i) { return i == 7; }));
        CHECK_FALSE(From(bid).Any([](auto i) { return i > 10; }));
        CHECK(From(forw).Any());
        CHECK(From(forw).Any([](auto i) { return i == 4; }));
        CHECK_FALSE(From(forw).Any([](auto i) { return i > 10; }));
        CHECK(From(inp).Any());
        CHECK(From(inp).Any([](auto i) { return i == 4; }));
        CHECK_FALSE(From(inp).Any([](auto i) { return i > 10; }));
    }

    SECTION("Concat")
    {
        CHECK(From(ran).Concat(ran).SequenceEqual(std::vector<int>{1, 2, 3, 4, 5, 1, 2, 3, 4, 5}));
        CHECK(From(ran).Concat(bid).SequenceEqual(Enumerable::Range(1, 9)));
        CHECK(From(ran).Concat(forw).SequenceEqual(std::vector<int>{1, 2, 3, 4, 5, 3, 4, 5, 6, 7}));
        CHECK(From(ran).Concat(inp).SequenceEqual(std::vector<int>{1, 2, 3, 4, 5, -1, 0, 1, 2, 3, 4, 5, 6}));
        CHECK(From(bid).Concat(ran).SequenceEqual(std::vector<int>{6, 7, 8, 9, 1, 2, 3, 4, 5}));
        CHECK(From(bid).Concat(bid).SequenceEqual(std::vector<int>{6, 7, 8, 9, 6, 7, 8, 9}));
        CHECK(From(bid).Concat(forw).SequenceEqual(std::vector<int>{6, 7, 8, 9, 3, 4, 5, 6, 7}));
        CHECK(From(bid).Concat(inp).SequenceEqual(std::vector<int>{6, 7, 8, 9, -1, 0, 1, 2, 3, 4, 5, 6}));
        CHECK(From(forw).Concat(ran).SequenceEqual(std::vector<int>{3, 4, 5, 6, 7, 1, 2, 3, 4, 5}));
        CHECK(From(forw).Concat(bid).SequenceEqual(std::vector<int>{3, 4, 5, 6, 7, 6, 7, 8, 9}));
        CHECK(From(forw).Concat(forw).SequenceEqual(std::vector<int>{3, 4, 5, 6, 7, 3, 4, 5, 6, 7}));
        CHECK(From(forw).Concat(inp).SequenceEqual(std::vector<int>{3, 4, 5, 6, 7, -1, 0, 1, 2, 3, 4, 5, 6}));
        CHECK(From(inp).Concat(ran).SequenceEqual(std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}));
        CHECK(From(inp).Concat(bid).SequenceEqual(std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9}));
        CHECK(From(inp).Concat(forw).SequenceEqual(std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6, 7}));
        CHECK(From(inp).Concat(inp).SequenceEqual(std::vector<int>{-1, 0, 1, 2, 3, 4, 5, 6, -1, 0, 1, 2, 3, 4, 5, 6}));
    }

    SECTION("Contains")
    {
        CHECK(From(ran).Contains(4));
        CHECK_FALSE(From(ran).Contains(7));
        CHECK(From(bid).Contains(7));
        CHECK_FALSE(From(bid).Contains(4));
    }

    SECTION("Distinct")
    {
        std::vector<int> d = { 1, 2, 3, 4 };

        CHECK(From(u).Distinct().SequenceEqual(d));
        CHECK(From(u).Distinct(std::less<>()).SequenceEqual(d)); CHECK(From(u).Distinct(std::equal_to<>(), std::hash<int>()).SequenceEqual(d));
        CHECK(From(va).Distinct().SequenceEqual(std::vector<A>{{1}, {3}}, [](auto a1, auto a2) { return a1.a == a2.a; }));
        CHECK(From(vb).Distinct().SequenceEqual(std::vector<B>{{2}, {4}, {1}}));
    }

    SECTION("ElementAt")
    {
        CHECK(From(ran).ElementAt(3) == 4);
        CHECK(From(bid).ElementAt(2) == 8);
        CHECK(From(ran).ElementAtOrDefault(-1) == 0);
        CHECK(From(bid).ElementAtOrDefault(10) == 0);
        CHECK(From(ran).ElementAtOrDefault(3) == 4);
        CHECK(From(bid).ElementAtOrDefault(2) == 8);
    }

    SECTION("Empty")
    {
        CHECK_FALSE(Enumerable::Empty<std::vector<int>>().Any());
        CHECK(Enumerable::Empty<int>().DefaultIfEmpty(20).Count() == 1);
        CHECK(From(ran).DefaultIfEmpty().Count() == ran.size());
        CHECK(Enumerable::Empty<int>().DefaultIfEmpty(20).First() == 20);
    }

    SECTION("First")
    {
        CHECK(From(ran).First() == 1);
        CHECK(From(ran).Skip(23).FirstOrDefault() == 0);
    }

    SECTION("Last")
    {
        CHECK(From(ran).Last() == 5);
        CHECK(From(ran).Skip(23).LastOrDefault() == 0);
    }

    SECTION("MinMax")
    {
        CHECK(From(ran).Min() == 1);
        CHECK(From(ran).Max() == 5);
        CHECK(From(ran).Min(std::negate<>()) == -5);
        CHECK(From(ran).Max(std::negate<>()) == -1);
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
        CHECK(From(ran).Reverse().SequenceEqual(std::vector<int>{5, 4, 3, 2, 1}));
    }

    SECTION("Select")
    {
        CHECK(From(ran).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(ran).Count())));
        CHECK(From(ran).Select([](auto vi, auto i) { return vi; }).SequenceEqual(ran));
        CHECK(From(ran).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'1'", "'2'", "'3'", "'4'", "'5'"}));
    }

    SECTION("SequenceEqual")
    {
        CHECK_FALSE(From(ran).SequenceEqual(bid));
        CHECK(Enumerable::Range(1, 5).SequenceEqual(From(ran)));
    }

    SECTION("Skip")
    {
        CHECK(From(ran).Skip(3).Count() == ran.size() - 3);
        CHECK(From(ran).Skip(6).Count() == 0);
    }

    SECTION("SkipWhile")
    {
        CHECK(From(ran).SkipWhile([](auto i) { return i < 4; }).Count() == 2);
        CHECK(From(ran).SkipWhile([](auto i, auto j) { return j < 4; }).First() == 5);
    }

    SECTION("Sum")
    {
        CHECK(From(ran).Sum() == 15);
        CHECK(From(ran).Sum([](auto i) { return i * i; }) == 55);
    }

    SECTION("Take")
    {
        CHECK(From(ran).Take(3).Count() == 3);
        CHECK(From(ran).Take(10).Count() == ran.size());
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
        CHECK(From(ran).Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(From(ran).Where([](auto vi) { return vi % 2 == 0; })));
        CHECK(From(ran).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(ran).Take(3)));
        CHECK(From(ran).Where([](auto i) { return i % 2 == 0; }).SequenceEqual(std::vector<int>{2, 4}));
    }

    SECTION("Yield")
    {
        CHECK(testYield().Count() == 8);
        CHECK(testYield().SequenceEqual(Enumerable::Range(-1, 8)));
        CHECK(testYield().SequenceEqual(testYield()));
        CHECK(testYield().Take(100).SequenceEqual(testYield()));

        auto thrower = []
        {
            START_YIELDING(int)
            yield_return(1);
            yield_return(2);
            throw 3;
            END_YIELDING
        };

        CHECK_THROWS_AS(thrower().Count(), int);
    }

    SECTION("Zip")
    {
        CHECK(From(ran).Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 6}, {2, 7}, {3, 8}, {4, 9}}));
    }
}
