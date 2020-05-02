using namespace Linqpp;

struct A
{
    test_t a;
    bool operator<(A a) const { return this->a < a.a; }
};
struct B
{
    test_t b;
    bool operator==(B b) const { return this->b == b.b; }
};


std::vector<test_t> ran = { 1, 2, 3, 4, 5 };
std::list<test_t> bid = { 6, 7, 8, 9 };
std::forward_list<test_t> forw = { 3, 4, 5, 6, 7 };

auto inp = []
{
    START_YIELDING(test_t)

    yield_return(-1);
    for (int i = 0; i < 7; ++i)
        yield_return(i);

    END_YIELDING
};

std::vector<test_t> u = { 1, 1, 2, 3, 3, 1, 2, 3, 4 };
std::vector<A> va = { A{1}, A{3}, A{3}, A{1} };
std::vector<B> vb = { B{2}, B{4}, B{1}, B{2}, B{1} };

SECTION("Aggregate")
{
    CHECK(From(ran).Aggregate(std::plus<>()) == 15);
    CHECK(From(bid).Aggregate(std::plus<>()) == 30);
    CHECK(From(forw).Aggregate(std::plus<>()) == 25);
    CHECK(From(inp()).Aggregate(std::plus<>()) == 20);

    using std::to_string;

    auto plus = [](std::string s, test_t i) { return s + to_string(i); };
    CHECK(From(ran).Aggregate(std::string(""), plus) == "12345");
    CHECK(From(bid).Aggregate(std::string(""), plus) == "6789");
    CHECK(From(forw).Aggregate(std::string(""), plus) == "34567");
    CHECK(From(inp()).Aggregate(std::string(""), plus) == "-10123456");

    auto to_int = [](std::string s) { return std::stoi(s); };
    CHECK(From(ran).Aggregate(std::string(""), plus, to_int) == 12345);
    CHECK(From(bid).Aggregate(std::string(""), plus, to_int) == 6789);
    CHECK(From(forw).Aggregate(std::string(""), plus, to_int) == 34567);
    CHECK(From(inp()).Aggregate(std::string(""), plus, to_int) == -10123456);
}

SECTION("All")
{
    CHECK_FALSE(From(ran).All([](auto i) { return i < 4; }));
    CHECK(From(ran).All([](auto i) { return i < 10; }));
    CHECK_FALSE(From(bid).All([](auto i) { return i < 4; }));
    CHECK(From(bid).All([](auto i) { return i < 10; }));
    CHECK_FALSE(From(forw).All([](auto i) { return i < 4; }));
    CHECK(From(forw).All([](auto i) { return i < 10; }));
    CHECK_FALSE(From(inp()).All([](auto i) { return i < 2; }));
    CHECK(From(inp()).All([](auto i) { return i < 10; }));
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

    CHECK(From(inp()).Any());
    CHECK(From(inp()).Any([](auto i) { return i == 4; }));
    CHECK_FALSE(From(inp()).Take(0).Any());
    CHECK_FALSE(From(inp()).Any([](auto i) { return i > 10; }));
}

SECTION("Concat")
{
    CHECK(From(ran).Concat(ran).SequenceEqual(std::vector<test_t>{1, 2, 3, 4, 5, 1, 2, 3, 4, 5}));
    CHECK(From(ran).Concat(bid).SequenceEqual(Enumerable::Range(1, 9).StaticCast<test_t>()));
    CHECK(From(ran).Concat(forw).SequenceEqual(std::vector<test_t>{1, 2, 3, 4, 5, 3, 4, 5, 6, 7}));
    CHECK(From(ran).Concat(inp()).SequenceEqual(std::vector<test_t>{1, 2, 3, 4, 5, -1, 0, 1, 2, 3, 4, 5, 6}));

    CHECK(From(bid).Concat(ran).SequenceEqual(std::vector<test_t>{6, 7, 8, 9, 1, 2, 3, 4, 5}));
    CHECK(From(bid).Concat(bid).SequenceEqual(std::vector<test_t>{6, 7, 8, 9, 6, 7, 8, 9}));
    CHECK(From(bid).Concat(forw).SequenceEqual(std::vector<test_t>{6, 7, 8, 9, 3, 4, 5, 6, 7}));
    CHECK(From(bid).Concat(inp()).SequenceEqual(std::vector<test_t>{6, 7, 8, 9, -1, 0, 1, 2, 3, 4, 5, 6}));

    CHECK(From(forw).Concat(ran).SequenceEqual(std::vector<test_t>{3, 4, 5, 6, 7, 1, 2, 3, 4, 5}));
    CHECK(From(forw).Concat(bid).SequenceEqual(std::vector<test_t>{3, 4, 5, 6, 7, 6, 7, 8, 9}));
    CHECK(From(forw).Concat(forw).SequenceEqual(std::vector<test_t>{3, 4, 5, 6, 7, 3, 4, 5, 6, 7}));
    CHECK(From(forw).Concat(inp()).SequenceEqual(std::vector<test_t>{3, 4, 5, 6, 7, -1, 0, 1, 2, 3, 4, 5, 6}));

    CHECK(From(inp()).Concat(ran).SequenceEqual(std::vector<test_t>{-1, 0, 1, 2, 3, 4, 5, 6, 1, 2, 3, 4, 5}));
    CHECK(From(inp()).Concat(bid).SequenceEqual(std::vector<test_t>{-1, 0, 1, 2, 3, 4, 5, 6, 6, 7, 8, 9}));
    CHECK(From(inp()).Concat(forw).SequenceEqual(std::vector<test_t>{-1, 0, 1, 2, 3, 4, 5, 6, 3, 4, 5, 6, 7}));
    CHECK(From(inp()).Concat(inp()).SequenceEqual(std::vector<test_t>{-1, 0, 1, 2, 3, 4, 5, 6, -1, 0, 1, 2, 3, 4, 5, 6}));
}

SECTION("Contains")
{
    CHECK(From(ran).Contains(test_t(4)));
    CHECK_FALSE(From(ran).Contains(test_t(7)));

    CHECK(From(bid).Contains(test_t(7)));
    CHECK_FALSE(From(bid).Contains(test_t(4)));

    CHECK(From(forw).Contains(test_t(4)));
    CHECK_FALSE(From(forw).Contains(test_t(8)));

    CHECK(From(inp()).Contains(test_t(4)));
    CHECK_FALSE(From(inp()).Contains(test_t(8)));
}

SECTION("DefaultIfEmpty")
{
    CHECK(Enumerable::Empty<test_t>().DefaultIfEmpty(20).Count() == 1);
    CHECK(Enumerable::Empty<test_t>().DefaultIfEmpty(20).First() == test_t(20));

    CHECK(From(ran).DefaultIfEmpty().Count() == ran.size());
    CHECK(From(bid).DefaultIfEmpty().Count() == bid.size());
    CHECK(From(forw).DefaultIfEmpty().Count() == From(forw).Count());
    CHECK(From(inp()).DefaultIfEmpty().Count() == inp().Count());
}

SECTION("Distinct")
{
    std::vector<test_t> d = { 1, 2, 3, 4 };

    CHECK(From(u).Distinct().SequenceEqual(d));
    CHECK(From(u).Distinct(std::less<>()).SequenceEqual(d));
    CHECK(From(u).Distinct(std::equal_to<>(), std::hash<test_t>()).SequenceEqual(d));
    CHECK(From(va).Distinct().SequenceEqual(std::vector<A>{{1}, {3}}, [](auto a1, auto a2) { return a1.a == a2.a; }));
    CHECK(From(vb).Distinct().SequenceEqual(std::vector<B>{{2}, {4}, {1}}));

    CHECK(From(ran).Distinct().SequenceEqual(ran));
    CHECK(From(bid).Distinct().SequenceEqual(bid));
    CHECK(From(forw).Distinct().SequenceEqual(forw));
    CHECK(From(inp()).Distinct().SequenceEqual(inp()));
}

SECTION("DynamicCast")
{
    struct Base
    {
        virtual ~Base() { }
        bool operator==(Base const& other) const { return this == &other; }
    };
    struct Derived1 : Base
    {
        bool operator==(Derived1 const& other) const { return this == &other; }
    };
    struct Derived2 : Base
    { };

    constexpr size_t size = 9;

    SECTION("Pointer")
    {
        std::vector<Derived1*> bran(size, new Derived1);
        auto aran = From(bran).StaticCast<Base*>();
        std::list<Derived1*> bbid(size, new Derived1);
        auto abid = From(bbid).StaticCast<Base*>();
        std::forward_list<Derived1*> bforw(size, new Derived1);
        auto aforw = From(bforw).StaticCast<Base*>();
        auto binp = [&]
        { 
            START_YIELDING(Derived1*)
            for (size_t i = 0; i < size; ++i)
                yield_return(bran[i]);
            END_YIELDING
        };

        CHECK(aran.DynamicCast<Derived1*>().SequenceEqual(bran));
        CHECK(abid.DynamicCast<Derived1*>().SequenceEqual(bbid));
        CHECK(aforw.DynamicCast<Derived1*>().SequenceEqual(bforw));
        CHECK(binp().StaticCast<Base*>().DynamicCast<Derived1*>().SequenceEqual(binp()));

        auto nullC = Enumerable::Repeat((Derived2*)nullptr, size);

        CHECK(aran.DynamicCast<Derived2*>().SequenceEqual(nullC));
        CHECK(abid.DynamicCast<Derived2*>().SequenceEqual(nullC));
        CHECK(aforw.DynamicCast<Derived2*>().SequenceEqual(nullC));
        CHECK(binp().StaticCast<Base*>().DynamicCast<Derived2*>().SequenceEqual(nullC));
    }

    SECTION("Reference")
    {
        std::vector<Derived1> bran(size, Derived1());
        auto branPointers = From(bran).Select([](auto& b) { return &b; }).ToVector();

        auto aran = From(bran).StaticCast<Base&>();

        std::list<Derived1> bbid(size, Derived1());
        auto abid = From(bbid).StaticCast<Base&>();

        std::forward_list<Derived1> bforw(size, Derived1());
        auto aforw = From(bforw).StaticCast<Base&>();

        auto binp = [&]
        {
            START_YIELDING(Derived1*)
            for (size_t i = 0; i < size; ++i)
                yield_return(branPointers[i]);
            END_YIELDING
        };

        auto ainp = [&]
        {
            START_YIELDING(Base*)
            for (size_t i = 0; i < size; ++i)
                yield_return(branPointers[i]);
            END_YIELDING
        };

        auto restartDerefInput = [](auto yieldFunc) { return yieldFunc().Select([] (auto pA) -> decltype(auto) { return *pA; }); };

        CHECK(aran.DynamicCast<Derived1&>().SequenceEqual(bran));
        CHECK(abid.DynamicCast<Derived1&>().SequenceEqual(bbid));
        CHECK(aforw.DynamicCast<Derived1&>().SequenceEqual(bforw));
        CHECK(restartDerefInput(ainp).DynamicCast<Derived1&>().SequenceEqual(restartDerefInput(binp)));

        CHECK_THROWS_AS(aran.DynamicCast<Derived2&>().SequenceEqual(aran), std::bad_cast);
        CHECK_THROWS_AS(abid.DynamicCast<Derived2&>().SequenceEqual(abid), std::bad_cast);
        CHECK_THROWS_AS(aforw.DynamicCast<Derived2&>().SequenceEqual(aforw), std::bad_cast);
        CHECK_THROWS_AS(restartDerefInput(ainp).DynamicCast<Derived2&>().SequenceEqual(restartDerefInput(ainp)), std::bad_cast);
    }
}

SECTION("ElementAt")
{
    CHECK(From(ran).ElementAt(3) == 4);
    CHECK(From(bid).ElementAt(2) == 8);
    CHECK(From(forw).ElementAt(3) == 6);
    CHECK(From(inp()).ElementAt(2) == 1);

    CHECK(From(ran).ElementAtOrDefault(3) == 4);
    CHECK(From(bid).ElementAtOrDefault(2) == 8);
    CHECK(From(forw).ElementAtOrDefault(3) == 6);
    CHECK(From(inp()).ElementAtOrDefault(2) == 1);

    CHECK(From(ran).ElementAtOrDefault(-1) == 0);
    CHECK(From(ran).ElementAtOrDefault(10) == 0);
    CHECK(From(bid).ElementAtOrDefault(-1) == 0);
    CHECK(From(bid).ElementAtOrDefault(10) == 0);
    CHECK(From(forw).ElementAtOrDefault(-1) == 0);
    CHECK(From(forw).ElementAtOrDefault(10) == 0);
    CHECK(From(inp()).ElementAtOrDefault(-1) == 0);
    CHECK(From(inp()).ElementAtOrDefault(10) == 0);
}

SECTION("Empty")
{
    CHECK_FALSE(Enumerable::Empty<test_t>().Any());
    CHECK_FALSE(Enumerable::Empty<double>().Any());
    CHECK_FALSE(Enumerable::Empty<std::vector<test_t>>().Any());
}

SECTION("First")
{
    CHECK(From(ran).First() == 1);
    CHECK(From(bid).First() == 6);
    CHECK(From(forw).First() == 3);
    CHECK(From(inp()).First() == -1);

    CHECK(From(ran).First([](auto i){ return i > 4; }) == 5);
    CHECK(From(bid).First([](auto i){ return static_cast<int>(i) % 2 == 1; }) == 7);
    CHECK(From(forw).First([](auto i){ return i > 4; }) == 5);
    CHECK(From(inp()).First([](auto i){ return i > 0; }) == 1);
}

SECTION("FirstOrDefault")
{
    CHECK(From(ran).FirstOrDefault() == 1);
    CHECK(From(bid).FirstOrDefault() == 6);
    CHECK(From(forw).FirstOrDefault() == 3);
    CHECK(From(inp()).FirstOrDefault() == -1);

    CHECK(From(ran).Skip(23).FirstOrDefault() == 0);
    CHECK(From(bid).Skip(23).FirstOrDefault() == 0);
    CHECK(From(forw).Skip(23).FirstOrDefault() == 0);
    CHECK(From(inp()).Skip(23).FirstOrDefault() == 0);

    CHECK(From(ran).FirstOrDefault([](auto i){ return i > 4; }) == 5);
    CHECK(From(bid).FirstOrDefault([](auto i){ return static_cast<int>(i) % 2 == 1; }) == 7);
    CHECK(From(forw).FirstOrDefault([](auto i){ return i > 4; }) == 5);
    CHECK(From(inp()).FirstOrDefault([](auto i){ return i > 0; }) == 1);

    CHECK(From(ran).FirstOrDefault([](auto i){ return i < 0; }) == 0);
    CHECK(From(bid).FirstOrDefault([](auto i){ return i < 1; }) == 0);
    CHECK(From(forw).FirstOrDefault([](auto i){ return i < 1; }) == 0);
    CHECK(From(inp()).FirstOrDefault([](auto i){ return i > 10; }) == 0);
}

SECTION("Last")
{
    CHECK(From(ran).Last() == 5);
    CHECK(From(bid).Last() == 9);
    CHECK(From(forw).Last() == 7);
    CHECK(From(inp()).Last() == 6);

    CHECK(From(ran).Last([](auto i){ return i < 4; }) == 3);
    CHECK(From(bid).Last([](auto i){ return static_cast<int>(i) % 2 == 0; }) == 8);
    CHECK(From(forw).Last([](auto i){ return i < 5; }) == 4);
    CHECK(From(inp()).Last([](auto i){ return i < 5; }) == 4);
}

SECTION("LastOrDefault")
{
    CHECK(From(ran).LastOrDefault() == 5);
    CHECK(From(bid).LastOrDefault() == 9);
    CHECK(From(forw).LastOrDefault() == 7);
    CHECK(From(inp()).LastOrDefault() == 6);

    CHECK(From(ran).Skip(23).LastOrDefault() == 0);
    CHECK(From(bid).Skip(23).LastOrDefault() == 0);
    CHECK(From(forw).Skip(23).LastOrDefault() == 0);
    CHECK(From(inp()).Skip(23).LastOrDefault() == 0);

    CHECK(From(ran).LastOrDefault([](auto i){ return i < 4; }) == 3);
    CHECK(From(bid).LastOrDefault([](auto i){ return static_cast<int>(i) % 2 == 0; }) == 8);
    CHECK(From(forw).LastOrDefault([](auto i){ return i < 5; }) == 4);
    CHECK(From(inp()).LastOrDefault([](auto i){ return i < 5; }) == 4);

    CHECK(From(ran).LastOrDefault([](auto i){ return i > 10; }) == 0);
    CHECK(From(bid).LastOrDefault([](auto i){ return i < 1; }) == 0);
    CHECK(From(forw).LastOrDefault([](auto i){ return i < 0; }) == 0);
    CHECK(From(inp()).LastOrDefault([](auto i){ return i > 10; }) == 0);
}

SECTION("Max")
{
    CHECK(From(ran).Max() == 5);
    CHECK(From(bid).Max() == 9);
    CHECK(From(forw).Max() == 7);
    CHECK(From(inp()).Max() == 6);

    CHECK(From(ran).Max(std::negate<>()) == -1);
    CHECK(From(bid).Max(std::negate<>()) == -6);
    CHECK(From(forw).Max(std::negate<>()) == -3);
    CHECK(From(inp()).Max(std::negate<>()) == 1);
}

SECTION("Min")
{
    CHECK(From(ran).Min() == 1);
    CHECK(From(bid).Min() == 6);
    CHECK(From(forw).Min() == 3);
    CHECK(From(inp()).Min() == -1);

    CHECK(From(ran).Min(std::negate<>()) == -5);
    CHECK(From(bid).Min(std::negate<>()) == -9);
    CHECK(From(forw).Min(std::negate<>()) == -7);
    CHECK(From(inp()).Min(std::negate<>()) == -6);
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
    CHECK(From(inp()).OrderBy([](auto a) { return a; }).SequenceEqual(inp()));

    CHECK(From(ran).OrderByDescending([](auto a) { return a; }).SequenceEqual(From(ran).Reverse()));
    CHECK(From(bid).OrderByDescending([](auto a) { return a; }).SequenceEqual(From(bid).Reverse()));
    CHECK(From(forw).OrderByDescending([](auto a) { return a; }).SequenceEqual(From(forw).Reverse()));
    CHECK(From(inp()).OrderByDescending([](auto a) { return a; }).SequenceEqual(inp().Reverse()));
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
    CHECK(From(ran).Reverse().SequenceEqual(std::vector<test_t>{5, 4, 3, 2, 1}));
    CHECK(From(bid).Reverse().SequenceEqual(std::vector<test_t>{9, 8, 7, 6}));
    CHECK(From(forw).Reverse().SequenceEqual(std::vector<test_t>{7, 6, 5, 4, 3}));
    CHECK(From(inp()).Reverse().SequenceEqual(std::vector<test_t>{6, 5, 4, 3, 2, 1, 0, -1}));
}

SECTION("Select")
{
    using std::to_string;

    CHECK(From(ran).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(ran).Count())));
    CHECK(From(ran).Select([](auto vi, auto i) { return vi; }).SequenceEqual(ran));
    CHECK(From(ran).Select([](auto i) { return '\'' + to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'1'", "'2'", "'3'", "'4'", "'5'"}));

    CHECK(From(bid).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(bid).Count())));
    CHECK(From(bid).Select([](auto vi, auto i) { return vi; }).SequenceEqual(bid));
    CHECK(From(bid).Select([](auto i) { return '\'' + to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'6'", "'7'", "'8'", "'9'"}));

    CHECK(From(forw).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(forw).Count())));
    CHECK(From(forw).Select([](auto vi, auto i) { return vi; }).SequenceEqual(forw));
    CHECK(From(forw).Select([](auto i) { return '\'' + to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'3'", "'4'", "'5'", "'6'", "'7'"}));

    CHECK(Enumerable::Range(0, From(inp()).Count()).Count() == 8);
    CHECK(From(inp()).Select([](auto vi, auto i) { return i; }).Count() == 8);
    CHECK(From(inp()).Select([](auto vi, auto i) { return i; }).SequenceEqual(Enumerable::Range(0, From(inp()).Count())));
    CHECK(From(inp()).Select([](auto vi, auto i) { return vi; }).SequenceEqual(inp()));
    CHECK(From(inp()).Select([](auto i) { return '\'' + to_string(i) + '\''; }).SequenceEqual(std::vector<std::string>{"'-1'", "'0'", "'1'", "'2'", "'3'", "'4'", "'5'", "'6'"}));
}

SECTION("SequenceEqual")
{
    CHECK_FALSE(From(ran).SequenceEqual(bid));
    CHECK_FALSE(From(ran).SequenceEqual(forw));
    CHECK_FALSE(From(ran).SequenceEqual(inp()));

    CHECK_FALSE(From(bid).SequenceEqual(ran));
    CHECK_FALSE(From(bid).SequenceEqual(forw));
    CHECK_FALSE(From(bid).SequenceEqual(inp()));

    CHECK_FALSE(From(forw).SequenceEqual(ran));
    CHECK_FALSE(From(forw).SequenceEqual(bid));
    CHECK_FALSE(From(forw).SequenceEqual(inp()));

    CHECK_FALSE(From(inp()).SequenceEqual(ran));
    CHECK_FALSE(From(inp()).SequenceEqual(bid));
    CHECK_FALSE(From(inp()).SequenceEqual(forw));

    CHECK(Enumerable::Range(1, 5).StaticCast<test_t>().SequenceEqual(From(ran)));
    CHECK(Enumerable::Range(6, 4).StaticCast<test_t>().SequenceEqual(From(bid)));
    CHECK(Enumerable::Range(3, 5).StaticCast<test_t>().SequenceEqual(From(forw)));
    CHECK(Enumerable::Range(-1, 8).StaticCast<test_t>().SequenceEqual(From(inp())));

    CHECK(From(ran).SequenceEqual(Enumerable::Range(1, 5).StaticCast<test_t>()));
    CHECK(From(bid).SequenceEqual(Enumerable::Range(6, 4).StaticCast<test_t>()));
    CHECK(From(forw).SequenceEqual(Enumerable::Range(3, 5).StaticCast<test_t>()));
    CHECK(From(inp()).SequenceEqual(Enumerable::Range(-1, 8).StaticCast<test_t>()));
}

SECTION("Skip")
{
    CHECK(From(ran).Skip(3).Count() == ran.size() - 3);
    CHECK(From(ran).Skip(6).Count() == 0);

    CHECK(From(bid).Skip(3).Count() == bid.size() - 3);
    CHECK(From(bid).Skip(6).Count() == 0);

    CHECK(From(forw).Skip(3).Count() == From(forw).Count() - 3);
    CHECK(From(forw).Skip(6).Count() == 0);

    CHECK(From(inp()).Skip(3).Count() == inp().Count() - 3);
    CHECK(From(inp()).Skip(9).Count() == 0);
}

SECTION("SkipWhile")
{
    CHECK(From(ran).SkipWhile([](auto vi) { return vi < 4; }).Count() == 2);
    CHECK(From(ran).SkipWhile([](auto vi, auto i) { return i < 4; }).First() == 5);

    CHECK(From(bid).SkipWhile([](auto vi) { return vi < 8; }).Count() == 2);
    CHECK(From(bid).SkipWhile([](auto vi, auto i) { return i < 3; }).First() == 9);

    CHECK(From(forw).SkipWhile([](auto vi) { return vi < 4; }).Count() == 4);
    CHECK(From(forw).SkipWhile([](auto vi, auto i) { return i < 4; }).First() == 7);

    CHECK(From(inp()).SkipWhile([](auto vi) { return vi < 4; }).Count() == 3);
    CHECK(From(inp()).SkipWhile([](auto vi, auto i) { return i < 4; }).First() == 3);
}

SECTION("StaticCast")
{
    CHECK(From(ran).StaticCast<short>().SequenceEqual(From(ran).Select([](auto i) { return static_cast<short>(i); })));
    CHECK(From(bid).StaticCast<short>().SequenceEqual(From(bid).Select([](auto i) { return static_cast<short>(i); })));
    CHECK(From(forw).StaticCast<short>().SequenceEqual(From(forw).Select([](auto i) { return static_cast<short>(i); })));
    CHECK(From(inp()).StaticCast<short>().SequenceEqual(From(inp()).Select([](auto i) { return static_cast<short>(i); })));
}

SECTION("Take")
{
    CHECK(From(ran).Take(3).Count() == 3);
    CHECK(From(ran).Take(10).Count() == ran.size());

    CHECK(From(bid).Take(3).Count() == 3);
    CHECK(From(bid).Take(10).Count() == bid.size());

    CHECK(From(forw).Take(3).Count() == 3);
    CHECK(From(forw).Take(10).Count() == From(forw).Count());

    CHECK(From(inp()).Take(3).Count() == 3);
    CHECK(From(inp()).Take(10).Count() == inp().Count());
}

SECTION("ToMap")
{
    SECTION("KeySelector")
    {
        std::map<test_t, test_t> mran = From(ran).ToMap([](auto i) { return i; });
        std::map<test_t, test_t> mbid = From(bid).ToMap([](auto i) { return i; });
        std::map<test_t, test_t> mforw = From(forw).ToMap([](auto i) { return i; });
        std::map<test_t, test_t> minp = From(inp()).ToMap([](auto i) { return i; });

        CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(mran));
        CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(mbid));
        CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(mforw));
        CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(minp));

        CHECK(From(ran).ToMap([](auto i) { return i; }).SequenceEqual(mran));
        CHECK(From(bid).ToMap([](auto i) { return i; }).SequenceEqual(mbid));
        CHECK(From(forw).ToMap([](auto i) { return i; }).SequenceEqual(mforw));
        CHECK(From(inp()).ToMap([](auto i) { return i; }).SequenceEqual(minp));
    }

    SECTION("KeySelector + KeyComparer")
    {
        std::map<test_t, test_t, std::greater<>> mran = From(ran).ToMap([](auto i) { return i; }, std::greater<>());
        std::map<test_t, test_t, std::greater<>> mbid = From(bid).ToMap([](auto i) { return i; }, std::greater<>());
        std::map<test_t, test_t, std::greater<>> mforw = From(forw).ToMap([](auto i) { return i; }, std::greater<>());
        std::map<test_t, test_t, std::greater<>> minp = From(inp()).ToMap([](auto i) { return i; }, std::greater<>());

        CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mran));
        CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mbid));
        CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mforw));
        CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(minp));

        minp = From(inp()).ToMap([](auto i) { return i; }, std::greater<>());

        CHECK(From(ran).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(mran));
        CHECK(From(bid).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(mbid));
        CHECK(From(forw).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(mforw));
        CHECK(From(inp()).ToMap([](auto i) { return i; }, std::greater<>()).SequenceEqual(minp));
    }

    SECTION("KeySelector + ValueSelector")
    {
        std::map<test_t, test_t> mran = From(ran).ToMap([](auto i) { return i; }, [](auto i) { return i; });
        std::map<test_t, test_t> mbid = From(bid).ToMap([](auto i) { return i; }, [](auto i) { return i; });
        std::map<test_t, test_t> mforw = From(forw).ToMap([](auto i) { return i; }, [](auto i) { return i; });
        std::map<test_t, test_t> minp = From(inp()).ToMap([](auto i) { return i; }, [](auto i) { return i; });

        CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(mran));
        CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(mbid));
        CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(mforw));
        CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).SequenceEqual(minp));

        minp = From(inp()).ToMap([](auto i) { return i; }, [](auto i) { return i; });

        CHECK(From(ran).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(mran));
        CHECK(From(bid).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(mbid));
        CHECK(From(forw).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(mforw));
        CHECK(From(inp()).ToMap([](auto i) { return i; }, [](auto i) { return i; }).SequenceEqual(minp));
    }

    SECTION("KeySelector + ValueSelector + KeyComparer")
    {
        std::map<test_t, test_t, std::greater<>> mran = From(ran).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());
        std::map<test_t, test_t, std::greater<>> mbid = From(bid).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());
        std::map<test_t, test_t, std::greater<>> mforw = From(forw).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());
        std::map<test_t, test_t, std::greater<>> minp = From(inp()).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>());

        CHECK(Enumerable::Range(1, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mran));
        CHECK(Enumerable::Range(6, 4).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mbid));
        CHECK(Enumerable::Range(3, 5).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(mforw));
        CHECK(Enumerable::Range(-1, 8).Select([](auto i) -> std::pair<const test_t, test_t> { return std::make_pair(i, i); }).Reverse().SequenceEqual(minp));

        minp = From(inp()).ToMap([](auto i) { return i; }, [](auto i) { return i; }, std::greater<>());

        CHECK(From(ran).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(mran));
        CHECK(From(bid).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(mbid));
        CHECK(From(forw).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(mforw));
        CHECK(From(inp()).ToMap([](auto i) { return i; },[](auto i) { return i; }, std::greater<>()).SequenceEqual(minp));
    }
}

SECTION("ToSet")
{
    SECTION("No Parameters")
    {
        std::set<test_t> vran = From(ran).ToSet();
        std::set<test_t> vbid = From(bid).ToSet();
        std::set<test_t> vforw = From(forw).ToSet();
        std::set<test_t> vinp = From(inp()).ToSet();

        CHECK(Enumerable::Range(1, 5).StaticCast<test_t>().SequenceEqual(vran));
        CHECK(Enumerable::Range(6, 4).StaticCast<test_t>().SequenceEqual(vbid));
        CHECK(Enumerable::Range(3, 5).StaticCast<test_t>().SequenceEqual(vforw));
        CHECK(Enumerable::Range(-1, 8).StaticCast<test_t>().SequenceEqual(vinp));

        vinp = From(inp()).ToSet();

        CHECK(From(ran).ToSet().SequenceEqual(vran));
        CHECK(From(bid).ToSet().SequenceEqual(vbid));
        CHECK(From(forw).ToSet().SequenceEqual(vforw));
        CHECK(From(inp()).ToSet().SequenceEqual(vinp));
    }

    SECTION("Comparer")
    {
        std::set<test_t, std::greater<>> vran = From(ran).ToSet(std::greater<>());
        std::set<test_t, std::greater<>> vbid = From(bid).ToSet(std::greater<>());
        std::set<test_t, std::greater<>> vforw = From(forw).ToSet(std::greater<>());
        std::set<test_t, std::greater<>> vinp = From(inp()).ToSet(std::greater<>());

        CHECK(Enumerable::Range(1, 5).StaticCast<test_t>().Reverse().SequenceEqual(vran));
        CHECK(Enumerable::Range(6, 4).StaticCast<test_t>().Reverse().SequenceEqual(vbid));
        CHECK(Enumerable::Range(3, 5).StaticCast<test_t>().Reverse().SequenceEqual(vforw));
        CHECK(Enumerable::Range(-1, 8).StaticCast<test_t>().Reverse().SequenceEqual(vinp));

        vinp = From(inp()).ToSet(std::greater<>());

        CHECK(From(ran).ToSet(std::greater<>()).SequenceEqual(vran));
        CHECK(From(bid).ToSet(std::greater<>()).SequenceEqual(vbid));
        CHECK(From(forw).ToSet(std::greater<>()).SequenceEqual(vforw));
        CHECK(From(inp()).ToSet(std::greater<>()).SequenceEqual(vinp));
    }
}

SECTION("ToVector")
{
    std::vector<test_t> vran = From(ran).ToVector();
    std::vector<test_t> vbid = From(bid).ToVector();
    std::vector<test_t> vforw = From(forw).ToVector();
    std::vector<test_t> vinp = From(inp()).ToVector();

    CHECK(Enumerable::Range(1, 5).StaticCast<test_t>().SequenceEqual(vran));
    CHECK(Enumerable::Range(6, 4).StaticCast<test_t>().SequenceEqual(vbid));
    CHECK(Enumerable::Range(3, 5).StaticCast<test_t>().SequenceEqual(vforw));
    CHECK(Enumerable::Range(-1, 8).StaticCast<test_t>().SequenceEqual(vinp));

    vinp = From(inp()).ToVector();

    CHECK(From(ran).ToVector().SequenceEqual(vran));
    CHECK(From(bid).ToVector().SequenceEqual(vbid));
    CHECK(From(forw).ToVector().SequenceEqual(vforw));
    CHECK(From(inp()).ToVector().SequenceEqual(vinp));
}

SECTION("Union")
{
    std::list<test_t> w = { 5, -1, 4, 3, 1 };
    std::vector<test_t> d = { 5, -1, 4, 3, 1, 2 };

    CHECK(From(w).Union(u).SequenceEqual(d));
    CHECK(From(w).Union(u, std::less<>()).SequenceEqual(d));
    CHECK(From(w).Union(u, std::equal_to<>(), std::hash<test_t>()).SequenceEqual(d));

    CHECK(From(ran).Union(ran).SequenceEqual(ran));
    CHECK(From(ran).Union(bid).SequenceEqual(From(ran).Concat(bid)));
    CHECK(From(ran).Union(forw).SequenceEqual(From(ran).Concat(From(forw).Skip(3))));
    CHECK(From(ran).Union(inp()).SequenceEqual(From(ran).Concat(std::vector<test_t>{-1, 0, 6})));

    CHECK(From(bid).Union(ran).SequenceEqual(From(bid).Concat(ran)));
    CHECK(From(bid).Union(bid).SequenceEqual(From(bid)));
    CHECK(From(bid).Union(forw).SequenceEqual(From(bid).Concat(From(forw).Take(3))));
    CHECK(From(bid).Union(inp()).SequenceEqual(From(bid).Concat(inp().Take(7))));

    CHECK(From(forw).Union(ran).SequenceEqual(From(forw).Concat(From(ran).Take(2))));
    CHECK(From(forw).Union(bid).SequenceEqual(From(forw).Concat(From(bid).Skip(2))));
    CHECK(From(forw).Union(forw).SequenceEqual(forw));
    CHECK(From(forw).Union(inp()).SequenceEqual(From(forw).Concat(inp().Take(4))));

    CHECK(From(inp()).Union(ran).SequenceEqual(inp()));
    CHECK(From(inp()).Union(bid).SequenceEqual(inp().Concat(From(bid).Skip(1))));
    CHECK(From(inp()).Union(forw).SequenceEqual(inp().Concat(std::vector<test_t>{7})));
    CHECK(From(inp()).Union(inp()).SequenceEqual(inp()));
}

SECTION("Where")
{
    CHECK(From(ran).Where([](auto vi, auto i) { return static_cast<int>(vi) % 2 == 0; }).SequenceEqual(From(ran).Where([](auto vi) { return static_cast<int>(vi) % 2 == 0; })));
    CHECK(From(ran).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(ran).Take(3)));
    CHECK(From(ran).Where([](auto i) { return static_cast<int>(i) % 2 == 0; }).SequenceEqual(std::vector<test_t>{2, 4}));

    CHECK(From(bid).Where([](auto vi, auto i) { return static_cast<int>(vi) % 2 == 0; }).SequenceEqual(From(bid).Where([](auto vi) { return static_cast<int>(vi) % 2 == 0; })));
    CHECK(From(bid).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(bid).Take(3)));
    CHECK(From(bid).Where([](auto i) { return static_cast<int>(i) % 2 == 0; }).SequenceEqual(std::vector<test_t>{6, 8}));

    CHECK(From(forw).Where([](auto vi, auto i) { return static_cast<int>(vi) % 2 == 0; }).SequenceEqual(From(forw).Where([](auto vi) { return static_cast<int>(vi) % 2 == 0; })));
    CHECK(From(forw).Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(From(forw).Take(3)));
    CHECK(From(forw).Where([](auto i) { return static_cast<int>(i) % 2 == 0; }).SequenceEqual(std::vector<test_t>{4, 6}));

    CHECK(inp().Where([](auto vi, auto i) { return static_cast<int>(vi) % 2 == 0; }).SequenceEqual(inp().Where([](auto vi) { return static_cast<int>(vi) % 2 == 0; })));
    CHECK(inp().Where([](auto vi, auto i) { return i < 3; }).SequenceEqual(inp().Take(3)));
    CHECK(inp().Where([](auto i) { return static_cast<int>(i) % 2 == 0; }).SequenceEqual(std::vector<test_t>{0, 2, 4, 6}));
}

SECTION("Zip")
{
    CHECK(From(ran).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}}));
    CHECK(From(ran).Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 6}, {2, 7}, {3, 8}, {4, 9}}));
    CHECK(From(ran).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 3}, {2, 4}, {3, 5}, {4, 6}, {5, 7}}));
    CHECK(From(ran).Zip(inp().Take(5), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, -1}, {2, 0}, {3, 1}, {4, 2}, {5, 3}}));

    CHECK(From(bid).Zip(From(ran).Take(4), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 1}, {7, 2}, {8, 3}, {9, 4}}));
    CHECK(From(bid).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 6}, {7, 7}, {8, 8}, {9, 9}}));
    CHECK(From(bid).Zip(From(forw).Take(4), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 3}, {7, 4}, {8, 5}, {9, 6}}));
    CHECK(From(bid).Zip(inp().Take(4), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, -1}, {7, 0}, {8, 1}, {9, 2}}));

    CHECK(From(forw).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 1}, {4, 2}, {5, 3}, {6, 4}, {7, 5}}));
    CHECK(From(forw).Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 6}, {4, 7}, {5, 8}, {6, 9}}));
    CHECK(From(forw).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}}));
    CHECK(From(forw).Zip(inp().Take(5), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, -1}, {4, 0}, {5, 1}, {6, 2}, {7, 3}}));

    CHECK(inp().Take(5).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 1}, {0, 2}, {1, 3}, {2, 4}, {3, 5}}));
    CHECK(inp().Take(4).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 6}, {0, 7}, {1, 8}, {2, 9}}));
    CHECK(inp().Take(5).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 3}, {0, 4}, {1, 5}, {2, 6}, {3, 7}}));
    CHECK(inp().Zip(inp(), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, -1}, {0, 0}, {1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}}));


    CHECK(From(ran).Zip(From(ran).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 1}, {2, 2}, {3, 3}}));
    CHECK(From(ran).Take(3).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 1}, {2, 2}, {3, 3}}));

    CHECK(From(ran).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 6}, {2, 7}, {3, 8}, {4, 9}}));
    CHECK(From(ran).Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 6}, {2, 7}, {3, 8}}));

    CHECK(From(ran).Zip(From(forw).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 3}, {2, 4}, {3, 5}}));
    CHECK(From(ran).Take(3).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, 3}, {2, 4}, {3, 5}}));

    CHECK(From(ran).Zip(inp(), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, -1}, {2, 0}, {3, 1}, {4, 2}, {5, 3}}));
    CHECK(From(ran).Zip(inp().Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{1, -1}, {2, 0}, {3, 1}}));

    CHECK(From(bid).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 1}, {7, 2}, {8, 3}, {9, 4}}));
    CHECK(From(bid).Zip(From(ran).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 1}, {7, 2}, {8, 3}}));

    CHECK(From(bid).Zip(From(bid).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 6}, {7, 7}, {8, 8}}));
    CHECK(From(bid).Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 6}, {7, 7}, {8, 8}}));

    CHECK(From(bid).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 3}, {7, 4}, {8, 5}, {9, 6}}));
    CHECK(From(bid).Zip(From(forw).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, 3}, {7, 4}, {8, 5}}));

    CHECK(From(bid).Zip(inp(), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, -1}, {7, 0}, {8, 1}, {9, 2}}));
    CHECK(From(bid).Zip(inp().Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{6, -1}, {7, 0}, {8, 1}}));

    CHECK(From(forw).Zip(From(ran).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 1}, {4, 2}, {5, 3}}));
    CHECK(From(forw).Take(3).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 1}, {4, 2}, {5, 3}}));
    
    CHECK(From(forw).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 6}, {4, 7}, {5, 8}, {6, 9}}));
    CHECK(From(forw).Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 6}, {4, 7}, {5, 8}}));

    CHECK(From(forw).Zip(From(forw).Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 3}, {4, 4}, {5, 5}}));
    CHECK(From(forw).Take(3).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, 3}, {4, 4}, {5, 5}}));

    CHECK(From(forw).Zip(inp(), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, -1}, {4, 0}, {5, 1}, {6, 2}, {7, 3}}));
    CHECK(From(forw).Zip(inp().Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{3, -1}, {4, 0}, {5, 1}}));

    CHECK(inp().Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 1}, {0, 2}, {1, 3}, {2, 4}, {3, 5}}));
    CHECK(inp().Take(3).Zip(ran, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 1}, {0, 2}, {1, 3}}));

    CHECK(inp().Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 6}, {0, 7}, {1, 8}, {2, 9}}));
    CHECK(inp().Take(3).Zip(bid, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 6}, {0, 7}, {1, 8}}));

    CHECK(inp().Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 3}, {0, 4}, {1, 5}, {2, 6}, {3, 7}}));
    CHECK(inp().Take(3).Zip(forw, [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, 3}, {0, 4}, {1, 5}}));

    CHECK(inp().Zip(inp().Take(3), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, -1}, {0, 0}, {1, 1}}));
    CHECK(inp().Take(3).Zip(inp(), [](auto i, auto j) { return std::make_pair(i, j); })
            .SequenceEqual(std::vector<std::pair<test_t, test_t>>{{-1, -1}, {0, 0}, {1, 1}}));
}
