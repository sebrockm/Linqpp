#include <complex>
#include <forward_list>
#include <limits>
#include <list>
#include <vector>

#include "Linqpp.hpp"
#include "catch.hpp"

using namespace Linqpp;

TEST_CASE("numeric tests")
{
    std::vector<int> ran = { 1, 2, 3, 4, 5 };
    std::list<int> bid = { 6, 7, 8, 9 };
    std::forward_list<int> forw = { 3, 4, 5, 6, 7 };
    auto inp = []
    {
        START_YIELDING(int)
        for (int i = -1; i < 7; ++i)
            yield_return(i);
        END_YIELDING
    }();

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
}
