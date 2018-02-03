#include <complex>
#include <forward_list>
#include <iostream>
#include <list>
#include <vector>

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
        CHECK_FALSE(From(ran).Take(0).Any());
        CHECK_FALSE(From(ran).Any([](auto i) { return i > 10; }));

        CHECK(From(bid).Any());
        CHECK(From(bid).Any([](auto i) { return i == 7; }));
        CHECK_FALSE(From(bid).Take(0).Any());
        CHECK_FALSE(From(bid).Any([](auto i) { return i > 10; }));

        CHECK(From(forw).Any());
        CHECK(From(forw).Any([](auto i) { return i == 4; }));
        CHECK_FALSE(From(forw).Take(0).Any());
        CHECK_FALSE(From(forw).Any([](auto i) { return i > 10; }));

        CHECK(From(inp).Any());
        CHECK(From(inp).Any([](auto i) { return i == 4; }));
        CHECK_FALSE(From(inp).Take(0).Any());
        CHECK_FALSE(From(inp).Any([](auto i) { return i > 10; }));
    }

    SECTION("Average")
    {
        CHECK(From(ran).Average() == Approx(3));
        CHECK(From(bid).Average() == Approx(7.5));
        CHECK(From(forw).Average() == Approx(5));
        CHECK(inp.Average() == Approx(2.5));

        long long n = 1'000'000;
        CHECK(Enumerable::Range(1, n).Average() == Approx((n + 1) / 2.0));
        CHECK(Enumerable::Range(0, n).Average() == Approx((n - 1) / 2.0));
        CHECK(Enumerable::Range(1, n).Reverse().Average() == Approx((n + 1) / 2.0));
        CHECK(Enumerable::Range(0, n).Reverse().Average() == Approx((n - 1) / 2.0));

        n *= 100;
        CHECK(Enumerable::Range(1, n).Average() == Approx((n + 1) / 2.0));
        CHECK(Enumerable::Range(0, n).Average() == Approx((n - 1) / 2.0));
        CHECK(Enumerable::Range(1, n).Reverse().Average() == Approx((n + 1) / 2.0));
        CHECK(Enumerable::Range(0, n).Reverse().Average() == Approx((n - 1) / 2.0));

        auto smallAndBig = Enumerable::Repeat(0.001, n).Concat(Enumerable::Repeat(n / 1'000.0, 1));
        auto avg = Approx(n / 500.0 / (n + 1));
        CHECK(smallAndBig.Average() == avg);
        CHECK(smallAndBig.Reverse().Average() == avg);

        CHECK(Enumerable::Repeat(std::numeric_limits<double>::max() / 2, 100).Average() == Approx(std::numeric_limits<double>::max() / 2));
        CHECK(Enumerable::Repeat(std::numeric_limits<double>::min() / 2, 100).Average() == Approx(std::numeric_limits<double>::min() / 2));

        using namespace std::literals::complex_literals;
        std::vector<std::complex<double>> vec = { 1.0 + 2.0i, 4.0 - 6.0i, -3.0 + 4.0i, -5.0, 2.0i };
        CHECK(From(vec).Average() == -0.6 + 0.4i);
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

        CHECK(From(forw).Contains(4));
        CHECK_FALSE(From(forw).Contains(8));

        CHECK(From(inp).Contains(4));
        CHECK_FALSE(From(inp).Contains(8));
    }

    SECTION("DefaultIfEmpty")
    {
        CHECK(Enumerable::Empty<int>().DefaultIfEmpty(20).Count() == 1);
        CHECK(Enumerable::Empty<int>().DefaultIfEmpty(20).First() == 20);

        CHECK(From(ran).DefaultIfEmpty().Count() == ran.size());
        CHECK(From(bid).DefaultIfEmpty().Count() == bid.size());
        CHECK(From(forw).DefaultIfEmpty().Count() == From(forw).Count());
        CHECK(From(inp).DefaultIfEmpty().Count() == inp.Count());
    }

    SECTION("Distinct")
    {
        std::vector<int> d = { 1, 2, 3, 4 };

        CHECK(From(u).Distinct().SequenceEqual(d));
        CHECK(From(u).Distinct(std::less<>()).SequenceEqual(d));
        CHECK(From(u).Distinct(std::equal_to<>(), std::hash<int>()).SequenceEqual(d));
        CHECK(From(va).Distinct().SequenceEqual(std::vector<A>{{1}, {3}}, [](auto a1, auto a2) { return a1.a == a2.a; }));
        CHECK(From(vb).Distinct().SequenceEqual(std::vector<B>{{2}, {4}, {1}}));

        CHECK(From(ran).Distinct().SequenceEqual(ran));
        CHECK(From(bid).Distinct().SequenceEqual(bid));
        CHECK(From(forw).Distinct().SequenceEqual(forw));
        CHECK(From(inp).Distinct().SequenceEqual(inp));
    }

    SECTION("ElementAt")
    {
        CHECK(From(ran).ElementAt(3) == 4);
        CHECK(From(bid).ElementAt(2) == 8);
        CHECK(From(forw).ElementAt(3) == 6);
        CHECK(From(inp).ElementAt(2) == 1);

        CHECK(From(ran).ElementAtOrDefault(3) == 4);
        CHECK(From(bid).ElementAtOrDefault(2) == 8);
        CHECK(From(forw).ElementAtOrDefault(3) == 6);
        CHECK(From(inp).ElementAtOrDefault(2) == 1);

        CHECK(From(ran).ElementAtOrDefault(-1) == 0);
        CHECK(From(ran).ElementAtOrDefault(10) == 0);
        CHECK(From(bid).ElementAtOrDefault(-1) == 0);
        CHECK(From(bid).ElementAtOrDefault(10) == 0);
        CHECK(From(forw).ElementAtOrDefault(-1) == 0);
        CHECK(From(forw).ElementAtOrDefault(10) == 0);
        CHECK(From(inp).ElementAtOrDefault(-1) == 0);
        CHECK(From(inp).ElementAtOrDefault(10) == 0);
    }

    SECTION("Empty")
    {
        CHECK_FALSE(Enumerable::Empty<int>().Any());
        CHECK_FALSE(Enumerable::Empty<double>().Any());
        CHECK_FALSE(Enumerable::Empty<std::vector<int>>().Any());
    }

    SECTION("First")
    {
        CHECK(From(ran).First() == 1);
        CHECK(From(bid).First() == 6);
        CHECK(From(forw).First() == 3);
        CHECK(From(inp).First() == -1);

        CHECK(From(ran).First([](auto i){ return i > 4; }) == 5);
        CHECK(From(bid).First([](auto i){ return i % 2 == 1; }) == 7);
        CHECK(From(forw).First([](auto i){ return i > 4; }) == 5);
        CHECK(From(inp).First([](auto i){ return i > 0; }) == 1);
    }

    SECTION("FirstOrDefault")
    {
        CHECK(From(ran).FirstOrDefault() == 1);
        CHECK(From(bid).FirstOrDefault() == 6);
        CHECK(From(forw).FirstOrDefault() == 3);
        CHECK(From(inp).FirstOrDefault() == -1);

        CHECK(From(ran).Skip(23).FirstOrDefault() == 0);
        CHECK(From(bid).Skip(23).FirstOrDefault() == 0);
        CHECK(From(forw).Skip(23).FirstOrDefault() == 0);
        CHECK(From(inp).Skip(23).FirstOrDefault() == 0);

        CHECK(From(ran).FirstOrDefault([](auto i){ return i > 4; }) == 5);
        CHECK(From(bid).FirstOrDefault([](auto i){ return i % 2 == 1; }) == 7);
        CHECK(From(forw).FirstOrDefault([](auto i){ return i > 4; }) == 5);
        CHECK(From(inp).FirstOrDefault([](auto i){ return i > 0; }) == 1);

        CHECK(From(ran).FirstOrDefault([](auto i){ return i < 0; }) == 0);
        CHECK(From(bid).FirstOrDefault([](auto i){ return i < 1; }) == 0);
        CHECK(From(forw).FirstOrDefault([](auto i){ return i < 1; }) == 0);
        CHECK(From(inp).FirstOrDefault([](auto i){ return i > 10; }) == 0);
    }

    SECTION("Last")
    {
        CHECK(From(ran).Last() == 5);
        CHECK(From(bid).Last() == 9);
        CHECK(From(forw).Last() == 7);
        CHECK(From(inp).Last() == 6);

        CHECK(From(ran).Last([](auto i){ return i < 4; }) == 3);
        CHECK(From(bid).Last([](auto i){ return i % 2 == 0; }) == 8);
        CHECK(From(forw).Last([](auto i){ return i < 5; }) == 4);
        CHECK(From(inp).Last([](auto i){ return i < 5; }) == 4);
    }

    SECTION("LastOrDefault")
    {
        CHECK(From(ran).LastOrDefault() == 5);
        CHECK(From(bid).LastOrDefault() == 9);
        CHECK(From(forw).LastOrDefault() == 7);
        CHECK(From(inp).LastOrDefault() == 6);

        CHECK(From(ran).Skip(23).LastOrDefault() == 0);
        CHECK(From(bid).Skip(23).LastOrDefault() == 0);
        CHECK(From(forw).Skip(23).LastOrDefault() == 0);
        CHECK(From(inp).Skip(23).LastOrDefault() == 0);

        CHECK(From(ran).LastOrDefault([](auto i){ return i < 4; }) == 3);
        CHECK(From(bid).LastOrDefault([](auto i){ return i % 2 == 0; }) == 8);
        CHECK(From(forw).LastOrDefault([](auto i){ return i < 5; }) == 4);
        CHECK(From(inp).LastOrDefault([](auto i){ return i < 5; }) == 4);

        CHECK(From(ran).LastOrDefault([](auto i){ return i > 10; }) == 0);
        CHECK(From(bid).LastOrDefault([](auto i){ return i < 1; }) == 0);
        CHECK(From(forw).LastOrDefault([](auto i){ return i < 0; }) == 0);
        CHECK(From(inp).LastOrDefault([](auto i){ return i > 10; }) == 0);
    }

    SECTION("Max")
    {
        CHECK(From(ran).Max() == 5);
        CHECK(From(bid).Max() == 9);
        CHECK(From(forw).Max() == 7);
        CHECK(From(inp).Max() == 6);

        CHECK(From(ran).Max(std::negate<>()) == -1);
        CHECK(From(bid).Max(std::negate<>()) == -6);
        CHECK(From(forw).Max(std::negate<>()) == -3);
        CHECK(From(inp).Max(std::negate<>()) == 1);
    }

    SECTION("Min")
    {
        CHECK(From(ran).Min() == 1);
        CHECK(From(bid).Min() == 6);
        CHECK(From(forw).Min() == 3);
        CHECK(From(inp).Min() == -1);

        CHECK(From(ran).Min(std::negate<>()) == -5);
        CHECK(From(bid).Min(std::negate<>()) == -9);
        CHECK(From(forw).Min(std::negate<>()) == -7);
        CHECK(From(inp).Min(std::negate<>()) == -6);
    }

    SECTION("OrderBy")
    {
        std::vector<B> vbs = { B{1}, B{1}, B{2}, B{2}, B{4} };
        std::vector<B> vbsd = { B{4}, B{2}, B{2}, B{1}, B{1} };

        CHECK(From(vb).OrderBy([](auto const& b) { return b.b; }).SequenceEqual(vbs));
        CHECK(From(vb).OrderByDescending([](auto const& b) { return b.b; }).SequenceEqual(vbsd));
        CHECK(From(vb).OrderBy([](auto const& b) { return b; }, [](auto const& b1, auto const& b2) { return b1.b < b2.b; }).SequenceEqual(vbs));
        CHECK(From(vb).OrderByDescending([](auto const& b) { return b; }, [](auto const& b1, auto const& b2) { return b1.b < b2.b; }).SequenceEqual(vbsd));

        CHECK(From(ran).OrderBy([](auto a) { return a; }).SequenceEqual(ran));
        CHECK(From(bid).OrderBy([](auto a) { return a; }).SequenceEqual(bid));
        CHECK(From(forw).OrderBy([](auto a) { return a; }).SequenceEqual(forw));
        CHECK(From(inp).OrderBy([](auto a) { return a; }).SequenceEqual(inp));

        CHECK(From(ran).OrderByDescending([](auto a) { return a; }).SequenceEqual(From(ran).Reverse()));
        CHECK(From(bid).OrderByDescending([](auto a) { return a; }).SequenceEqual(From(bid).Reverse()));
        CHECK(From(forw).OrderByDescending([](auto a) { return a; }).SequenceEqual(From(forw).Reverse()));
        CHECK(From(inp).OrderByDescending([](auto a) { return a; }).SequenceEqual(inp.Reverse()));
    }

    SECTION("Repeat")
    {
        auto rep = Enumerable::Repeat(1.234, 10);

        CHECK(rep.Count() == 10);
        CHECK(rep.All([](auto d) { return d == 1.234; }));

        auto rep2 = Enumerable::Repeat(1234, 1000);

        CHECK(rep2.Count() == 1000);
        CHECK(rep2.All([](auto d) { return d == 1234; }));
    }

    SECTION("Reverse")
    {
        CHECK(From(ran).Reverse().SequenceEqual(std::vector<int>{5, 4, 3, 2, 1}));
        CHECK(From(bid).Reverse().SequenceEqual(std::vector<int>{9, 8, 7, 6}));
        CHECK(From(forw).Reverse().SequenceEqual(std::vector<int>{7, 6, 5, 4, 3}));
        CHECK(From(inp).Reverse().SequenceEqual(std::vector<int>{6, 5, 4, 3, 2, 1, 0, -1}));
    }

    SECTION("Select")
    {
        CHECK(From(ran).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(ran).Count())));
        CHECK(From(ran).Select([](auto vi, auto i) { return vi; }).SequenceEqual(ran));
        CHECK(From(ran).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'1'", "'2'", "'3'", "'4'", "'5'"}));

        CHECK(From(bid).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(bid).Count())));
        CHECK(From(bid).Select([](auto vi, auto i) { return vi; }).SequenceEqual(bid));
        CHECK(From(bid).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'6'", "'7'", "'8'", "'9'"}));

        CHECK(From(forw).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(forw).Count())));
        CHECK(From(forw).Select([](auto vi, auto i) { return vi; }).SequenceEqual(forw));
        CHECK(From(forw).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'3'", "'4'", "'5'", "'6'", "'7'"}));

        CHECK(From(inp).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(inp).Count())));
        CHECK(From(inp).Select([](auto vi, auto i) { return vi; }).SequenceEqual(inp));
        CHECK(From(inp).Select([](auto i) { return '\'' + std::to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'-1'", "'0'", "'1'", "'2'", "'3'", "'4'", "'5'", "'6'"}));
    }

    SECTION("SequenceEqual")
    {
        CHECK_FALSE(From(ran).SequenceEqual(bid));
        CHECK_FALSE(From(ran).SequenceEqual(forw));
        CHECK_FALSE(From(ran).SequenceEqual(inp));

        CHECK_FALSE(From(bid).SequenceEqual(ran));
        CHECK_FALSE(From(bid).SequenceEqual(forw));
        CHECK_FALSE(From(bid).SequenceEqual(inp));

        CHECK_FALSE(From(forw).SequenceEqual(ran));
        CHECK_FALSE(From(forw).SequenceEqual(bid));
        CHECK_FALSE(From(forw).SequenceEqual(inp));

        CHECK_FALSE(From(inp).SequenceEqual(ran));
        CHECK_FALSE(From(inp).SequenceEqual(bid));
        CHECK_FALSE(From(inp).SequenceEqual(forw));

        CHECK(Enumerable::Range(1, 5).SequenceEqual(From(ran)));
        CHECK(Enumerable::Range(6, 4).SequenceEqual(From(bid)));
        CHECK(Enumerable::Range(3, 5).SequenceEqual(From(forw)));
        CHECK(Enumerable::Range(-1, 8).SequenceEqual(From(inp)));

        CHECK(From(ran).SequenceEqual(Enumerable::Range(1, 5)));
        CHECK(From(bid).SequenceEqual(Enumerable::Range(6, 4)));
        CHECK(From(forw).SequenceEqual(Enumerable::Range(3, 5)));
        CHECK(From(inp).SequenceEqual(Enumerable::Range(-1, 8)));
    }

    SECTION("Skip")
    {
        CHECK(From(ran).Skip(3).Count() == ran.size() - 3);
        CHECK(From(ran).Skip(6).Count() == 0);

        CHECK(From(bid).Skip(3).Count() == bid.size() - 3);
        CHECK(From(bid).Skip(6).Count() == 0);

        CHECK(From(forw).Skip(3).Count() == From(forw).Count() - 3);
        CHECK(From(forw).Skip(6).Count() == 0);

        CHECK(From(inp).Skip(3).Count() == inp.Count() - 3);
        CHECK(From(inp).Skip(9).Count() == 0);
    }

    SECTION("SkipWhile")
    {
        CHECK(From(ran).SkipWhile([](auto vi) { return vi < 4; }).Count() == 2);
        CHECK(From(ran).SkipWhile([](auto vi, auto i) { return i < 4; }).First() == 5);

        CHECK(From(bid).SkipWhile([](auto vi) { return vi < 8; }).Count() == 2);
        CHECK(From(bid).SkipWhile([](auto vi, auto i) { return i < 3; }).First() == 9);

        CHECK(From(forw).SkipWhile([](auto vi) { return vi < 4; }).Count() == 4);
        CHECK(From(forw).SkipWhile([](auto vi, auto i) { return i < 4; }).First() == 7);

        CHECK(From(inp).SkipWhile([](auto vi) { return vi < 4; }).Count() == 3);
        CHECK(From(inp).SkipWhile([](auto vi, auto i) { return i < 4; }).First() == 3);
    }

    SECTION("Sum")
    {
        CHECK(From(ran).Sum() == 15);
        CHECK(From(ran).Sum([](auto i) { return i * i; }) == 55);

        CHECK(From(bid).Sum() == 30);
        CHECK(From(bid).Sum([](auto i) { return i * i; }) == 230);

        CHECK(From(forw).Sum() == 25);
        CHECK(From(forw).Sum([](auto i) { return i * i; }) == 135);

        CHECK(From(inp).Sum() == 20);
        CHECK(From(inp).Sum([](auto i) { return i * i; }) == 92);

        using namespace std::literals::complex_literals;
        std::vector<std::complex<double>> vec = { 1.0 + 2.0i, 4.0 - 6.0i, -3.0 + 4.0i, -5.0, 2.0i };
        CHECK(From(vec).Sum() == -3.0 + 2.0i);
    }

    SECTION("Take")
    {
        CHECK(From(ran).Take(3).Count() == 3);
        CHECK(From(ran).Take(10).Count() == ran.size());

        CHECK(From(bid).Take(3).Count() == 3);
        CHECK(From(bid).Take(10).Count() == bid.size());

        CHECK(From(forw).Take(3).Count() == 3);
        CHECK(From(forw).Take(10).Count() == From(forw).Count());

        CHECK(From(inp).Take(3).Count() == 3);
        CHECK(From(inp).Take(10).Count() == inp.Count());
    }

    SECTION("ToMap")
    {
        SECTION("KeySelector")
        {
            std::map<int, int> mran = From(ran).ToMap([](auto i) { return i; });
            std::map<int, int> mbid = From(bid).ToMap([](auto i) { return i; });
            std::map<int, int> mforw = From(forw).ToMap([](auto i) { return i; });
            std::map<int, int> minp = From(inp).ToMap([](auto i) { return i; });

            CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(mran));
            CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(mbid));
            CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(mforw));
            CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(minp));

            CHECK(From(ran).ToMap([](auto i) { return i; }).SequenceEqual(mran));
            CHECK(From(bid).ToMap([](auto i) { return i; }).SequenceEqual(mbid));
            CHECK(From(forw).ToMap([](auto i) { return i; }).SequenceEqual(mforw));
            CHECK(From(inp).ToMap([](auto i) { return i; }).SequenceEqual(minp));
        }

        SECTION("KeySelector + KeyComparer")
        {
            std::map<int, int, std::greater<>> mran = From(ran).ToMap([](auto i) { return i; }, std::greater<>());
            std::map<int, int, std::greater<>> mbid = From(bid).ToMap([](auto i) { return i; }, std::greater<>());
            std::map<int, int, std::greater<>> mforw = From(forw).ToMap([](auto i) { return i; }, std::greater<>());
            std::map<int, int, std::greater<>> minp = From(inp).ToMap([](auto i) { return i; }, std::greater<>());

            CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mran));
            CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mbid));
            CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mforw));
            CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(minp));

            CHECK(From(ran).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(mran));
            CHECK(From(bid).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(mbid));
            CHECK(From(forw).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(mforw));
            CHECK(From(inp).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(minp));
        }

        SECTION("KeySelector + ValueSelector")
        {
            std::map<int, int> mran = From(ran).ToMap([](auto i) { return i; }, [](auto i) { return i; });
            std::map<int, int> mbid = From(bid).ToMap([](auto i) { return i; }, [](auto i) { return i; });
            std::map<int, int> mforw = From(forw).ToMap([](auto i) { return i; }, [](auto i) { return i; });
            std::map<int, int> minp = From(inp).ToMap([](auto i) { return i; }, [](auto i) { return i; });

            CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(mran));
            CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(mbid));
            CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(mforw));
            CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).SequenceEqual(minp));

            CHECK(From(ran).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(mran));
            CHECK(From(bid).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(mbid));
            CHECK(From(forw).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(mforw));
            CHECK(From(inp).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(minp));
        }

        SECTION("KeySelector + ValueSelector + KeyComparer")
        {
            std::map<int, int, std::greater<>> mran = From(ran).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());
            std::map<int, int, std::greater<>> mbid = From(bid).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());
            std::map<int, int, std::greater<>> mforw = From(forw).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());
            std::map<int, int, std::greater<>> minp = From(inp).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());

            CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mran));
            CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mbid));
            CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mforw));
            CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const int, int> { return std::make_pair(i, i); }).Reverse().SequenceEqual(minp));

            CHECK(From(ran).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(mran));
            CHECK(From(bid).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(mbid));
            CHECK(From(forw).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(mforw));
            CHECK(From(inp).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(minp));
        }
    }

    SECTION("ToVector")
    {
        std::vector<int> vran = From(ran).ToVector();
        std::vector<int> vbid = From(bid).ToVector();
        std::vector<int> vforw = From(forw).ToVector();
        std::vector<int> vinp = From(inp).ToVector();

        CHECK(Enumerable::Range(1, 5).SequenceEqual(vran));
        CHECK(Enumerable::Range(6, 4).SequenceEqual(vbid));
        CHECK(Enumerable::Range(3, 5).SequenceEqual(vforw));
        CHECK(Enumerable::Range(-1, 8).SequenceEqual(vinp));

        CHECK(From(ran).ToVector().SequenceEqual(vran));
        CHECK(From(bid).ToVector().SequenceEqual(vbid));
        CHECK(From(forw).ToVector().SequenceEqual(vforw));
        CHECK(From(inp).ToVector().SequenceEqual(vinp));
    }

    SECTION("Union")
    {
        std::list<int> w = { 5, -1, 4, 3, 1 };
        std::vector<int> d = { 5, -1, 4, 3, 1, 2 };

        CHECK(From(w).Union(u).SequenceEqual(d));
        CHECK(From(w).Union(u, std::less<>()).SequenceEqual(d));
        CHECK(From(w).Union(u, std::equal_to<>(), std::hash<int>()).SequenceEqual(d));

        CHECK(From(ran).Union(ran).SequenceEqual(ran));
        CHECK(From(ran).Union(bid).SequenceEqual(From(ran).Concat(bid)));
        CHECK(From(ran).Union(forw).SequenceEqual(From(ran).Concat(From(forw).Skip(3))));
        CHECK(From(ran).Union(inp).SequenceEqual(From(ran).Concat(std::vector<int>{-1, 0, 6})));

        CHECK(From(bid).Union(ran).SequenceEqual(From(bid).Concat(ran)));
        CHECK(From(bid).Union(bid).SequenceEqual(From(bid)));
        CHECK(From(bid).Union(forw).SequenceEqual(From(bid).Concat(From(forw).Take(3))));
        CHECK(From(bid).Union(inp).SequenceEqual(From(bid).Concat(inp.Take(7))));

        CHECK(From(forw).Union(ran).SequenceEqual(From(forw).Concat(From(ran).Take(2))));
        CHECK(From(forw).Union(bid).SequenceEqual(From(forw).Concat(From(bid).Skip(2))));
        CHECK(From(forw).Union(forw).SequenceEqual(forw));
        CHECK(From(forw).Union(inp).SequenceEqual(From(forw).Concat(inp.Take(4))));

        CHECK(From(inp).Union(ran).SequenceEqual(inp));
        CHECK(From(inp).Union(bid).SequenceEqual(inp.Concat(From(bid).Skip(1))));
        CHECK(From(inp).Union(forw).SequenceEqual(inp.Concat(Enumerable::Range(7, 1))));
        CHECK(From(inp).Union(inp).SequenceEqual(inp));
    }

    SECTION("Where")
    {
        CHECK(From(ran).Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(From(ran).Where([](auto vi) { return vi % 2 == 0; })));
        CHECK(From(ran).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(ran).Take(3)));
        CHECK(From(ran).Where([](auto i) { return i % 2 == 0; }).SequenceEqual(std::vector<int>{2, 4}));

        CHECK(From(bid).Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(From(bid).Where([](auto vi) { return vi % 2 == 0; })));
        CHECK(From(bid).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(bid).Take(3)));
        CHECK(From(bid).Where([](auto i) { return i % 2 == 0; }).SequenceEqual(std::vector<int>{6, 8}));

        CHECK(From(forw).Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(From(forw).Where([](auto vi) { return vi % 2 == 0; })));
        CHECK(From(forw).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(forw).Take(3)));
        CHECK(From(forw).Where([](auto i) { return i % 2 == 0; }).SequenceEqual(std::vector<int>{4, 6}));

        CHECK(inp.Where([](auto vi, auto i) { return vi % 2 == 0; }).SequenceEqual(inp.Where([](auto vi) { return vi % 2 == 0; })));
        CHECK((inp).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(inp.Take(3)));
        CHECK((inp).Where([](auto i) { return i % 2 == 0; }).SequenceEqual(std::vector<int>{0, 2, 4, 6}));
    }

    SECTION("Yield")
    {
        CHECK(testYield().Count() == 8);
        CHECK(testYield().SequenceEqual(Enumerable::Range(-1, 8)));
        CHECK(testYield().Take(100).SequenceEqual(testYield()));

        CHECK(testYield().SequenceEqual(testYield()));
        CHECK(testYield().SequenceEqual(From(testYield())));
        CHECK(testYield().SequenceEqual(inp));
        CHECK(testYield().SequenceEqual(From(inp)));

        CHECK(From(testYield()).SequenceEqual(testYield()));
        CHECK(From(testYield()).SequenceEqual(From(testYield())));
        CHECK(From(testYield()).SequenceEqual(inp));
        CHECK(From(testYield()).SequenceEqual(From(inp)));

        CHECK(inp.SequenceEqual(testYield()));
        CHECK(inp.SequenceEqual(From(testYield())));
        CHECK(inp.SequenceEqual(inp));
        CHECK(inp.SequenceEqual(From(inp)));

        CHECK(From(inp).SequenceEqual(testYield()));
        CHECK(From(inp).SequenceEqual(From(testYield())));
        CHECK(From(inp).SequenceEqual(inp));
        CHECK(From(inp).SequenceEqual(From(inp)));

        auto inp1 = testYield();
        auto inp2 = testYield();
        CHECK(inp1.Count() == inp2.Count());
        for (unsigned i = 0; i < inp.Count(); ++i)
        {
            CHECK(inp1.Take(i).Count() == i);
            CHECK(inp2.Take(i).Count() == i);
            CHECK(inp1.Take(i).SequenceEqual(inp2.Take(i)));

            CHECK(inp1.Select([](auto i) { return i; }).Take(i).Count() == i);
            CHECK(inp2.Select([](auto i) { return i; }).Take(i).Count() == i);
            CHECK(inp1.Select([](auto i) { return i; }).Take(i).SequenceEqual(inp2.Take(i)));
        }

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
        CHECK(From(ran).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}}));
        CHECK(From(ran).Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 6}, {2, 7}, {3, 8}, {4, 9}}));
        CHECK(From(ran).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 3}, {2, 4}, {3, 5}, {4, 6}, {5, 7}}));
        CHECK(From(ran).Zip(inp.Take(5), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, -1}, {2, 0}, {3, 1}, {4, 2}, {5, 3}}));

        CHECK(From(bid).Zip(From(ran).Take(4), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 1}, {7, 2}, {8, 3}, {9, 4}}));
        CHECK(From(bid).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 6}, {7, 7}, {8, 8}, {9, 9}}));
        CHECK(From(bid).Zip(From(forw).Take(4), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 3}, {7, 4}, {8, 5}, {9, 6}}));
        CHECK(From(bid).Zip(inp.Take(4), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, -1}, {7, 0}, {8, 1}, {9, 2}}));

        CHECK(From(forw).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 1}, {4, 2}, {5, 3}, {6, 4}, {7, 5}}));
        CHECK(From(forw).Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 6}, {4, 7}, {5, 8}, {6, 9}}));
        CHECK(From(forw).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}}));
        CHECK(From(forw).Zip(inp.Take(5), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, -1}, {4, 0}, {5, 1}, {6, 2}, {7, 3}}));

        CHECK(inp.Take(5).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 1}, {0, 2}, {1, 3}, {2, 4}, {3, 5}}));
        CHECK(inp.Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 6}, {0, 7}, {1, 8}, {2, 9}}));
        CHECK(inp.Take(5).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 3}, {0, 4}, {1, 5}, {2, 6}, {3, 7}}));
        CHECK(inp.Zip(inp, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, -1}, {0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}}));


        CHECK(From(ran).Zip(From(ran).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 1}, {2, 2}, {3, 3}}));
        CHECK(From(ran).Take(3).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 1}, {2, 2}, {3, 3}}));

        CHECK(From(ran).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 6}, {2, 7}, {3, 8}, {4, 9}}));
        CHECK(From(ran).Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 6}, {2, 7}, {3, 8}}));

        CHECK(From(ran).Zip(From(forw).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 3}, {2, 4}, {3, 5}}));
        CHECK(From(ran).Take(3).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, 3}, {2, 4}, {3, 5}}));

        CHECK(From(ran).Zip(inp, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, -1}, {2, 0}, {3, 1}, {4, 2}, {5, 3}}));
        CHECK(From(ran).Zip(inp.Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{1, -1}, {2, 0}, {3, 1}}));

        CHECK(From(bid).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 1}, {7, 2}, {8, 3}, {9, 4}}));
        CHECK(From(bid).Zip(From(ran).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 1}, {7, 2}, {8, 3}}));

        CHECK(From(bid).Zip(From(bid).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 6}, {7, 7}, {8, 8}}));
        CHECK(From(bid).Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 6}, {7, 7}, {8, 8}}));

        CHECK(From(bid).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 3}, {7, 4}, {8, 5}, {9, 6}}));
        CHECK(From(bid).Zip(From(forw).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, 3}, {7, 4}, {8, 5}}));

        CHECK(From(bid).Zip(inp, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, -1}, {7, 0}, {8, 1}, {9, 2}}));
        CHECK(From(bid).Zip(inp.Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{6, -1}, {7, 0}, {8, 1}}));

        CHECK(From(forw).Zip(From(ran).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 1}, {4, 2}, {5, 3}}));
        CHECK(From(forw).Take(3).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 1}, {4, 2}, {5, 3}}));
        
        CHECK(From(forw).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 6}, {4, 7}, {5, 8}, {6, 9}}));
        CHECK(From(forw).Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 6}, {4, 7}, {5, 8}}));

        CHECK(From(forw).Zip(From(forw).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 3}, {4, 4}, {5, 5}}));
        CHECK(From(forw).Take(3).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, 3}, {4, 4}, {5, 5}}));

        CHECK(From(forw).Zip(inp, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, -1}, {4, 0}, {5, 1}, {6, 2}, {7, 3}}));
        CHECK(From(forw).Zip(inp.Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{3, -1}, {4, 0}, {5, 1}}));

        CHECK(inp.Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 1}, {0, 2}, {1, 3}, {2, 4}, {3, 5}}));
        CHECK(inp.Take(3).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 1}, {0, 2}, {1, 3}}));

        CHECK(inp.Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 6}, {0, 7}, {1, 8}, {2, 9}}));
        CHECK(inp.Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 6}, {0, 7}, {1, 8}}));

        CHECK(inp.Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 3}, {0, 4}, {1, 5}, {2, 6}, {3, 7}}));
        CHECK(inp.Take(3).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, 3}, {0, 4}, {1, 5}}));

        CHECK(inp.Zip(inp.Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, -1}, {0, 0}, {1, 1}}));
        CHECK(inp.Take(3).Zip(inp, [](auto i, auto j) { return std::make_pair(i, j); })
                .SequenceEqual(std::vector<std::pair<int, int>>{{-1, -1}, {0, 0}, {1, 1}}));
    }
}
