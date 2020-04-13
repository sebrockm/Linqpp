#include <memory>
#include <vector>
#include <iostream>
#include <utility>

#include "Linqpp.hpp"
#include "catch.hpp"

using namespace Linqpp;

auto EmptyResult()
{
    START_YIELDING(int)
    END_YIELDING
}

auto EmptyResult(int a, float b)
{
    START_YIELDING(int)
    END_YIELDING
}

auto EmptyResult(float& a)
{
    START_YIELDING(int)
    END_YIELDING
}

auto SingleFunction()
{
    START_YIELDING(int)
    yield_return(1);
    END_YIELDING
}

auto SingleFunction(int i)
{
    START_YIELDING(int)
    yield_return(i);
    END_YIELDING
}

auto SingleFunction(float& i)
{
    START_YIELDING(float)
    yield_return(i);
    END_YIELDING
}

auto MultipleFunction()
{
    START_YIELDING(int)
    for (int i = 0; i < 100; ++i)
        yield_return(i);
    END_YIELDING
}

auto MultipleFunction(float f)
{
    START_YIELDING(float)
    for (int i = 0; i < 100; ++i)
        yield_return(i + f);
    END_YIELDING
}

auto MultipleFunction(int& a)
{
    START_YIELDING(int)
    for (int i = 0; i < 100; ++i)
        yield_return(i + a);
    END_YIELDING
}

auto HugeFunction()
{
    START_YIELDING(int)
    for (int i = 0; i < 1'000'000; ++i)
        yield_return(i);
    END_YIELDING
}

template <class T>
auto TemplateFunction()
{
    START_YIELDING(T)
    yield_return(T());
    yield_return(T());
    END_YIELDING
}

TEST_CASE("yield_return tests")
{
    SECTION("Without parameters")
    {
        SECTION("Single call")
        {
            CHECK(!EmptyResult().Any());
            CHECK(SingleFunction().First() == 1);
            CHECK(MultipleFunction().Count() == 100);
            CHECK(HugeFunction().Count() == 1'000'000);

            CHECK(TemplateFunction<double>().Last() == double());
            CHECK(TemplateFunction<std::vector<bool>>().First().size() == 0);
            CHECK(TemplateFunction<int*>().Count() == 2);
        }

        SECTION("Multiple calls")
        {
            for (int i = 0; i < 100; ++i)
            {
                CHECK(MultipleFunction().Count() == 100);
                CHECK(MultipleFunction().First() == 0);
                CHECK(MultipleFunction().Last() == 99);
            }
        }

        SECTION("Single call stored")
        {
            auto emptyResult = EmptyResult();
            auto singleFunction = SingleFunction();
            auto multipleFunction = MultipleFunction();
            auto hugeFunction = HugeFunction();
            auto templateFunctionDouble = TemplateFunction<double>();
            auto templateFunctionVecBool = TemplateFunction<std::vector<bool>>();
            auto templateFunctionIntPtr = TemplateFunction<int*>();

            CHECK(!emptyResult.Any());
            CHECK(singleFunction.First() == 1);
            CHECK(multipleFunction.Count() == 100);
            CHECK(hugeFunction.Count() == 1'000'000);

            CHECK(templateFunctionDouble.Last() == double());
            CHECK(templateFunctionVecBool.First().size() == 0);
            CHECK(templateFunctionIntPtr.Count() == 2);
        }

        SECTION("Equality")
        {
            CHECK(MultipleFunction().SequenceEqual(MultipleFunction()));

            auto multipleFunction = MultipleFunction();
            CHECK(multipleFunction.SequenceEqual(MultipleFunction()));

            multipleFunction = MultipleFunction();
            CHECK(MultipleFunction().SequenceEqual(multipleFunction));

            multipleFunction = MultipleFunction();
            auto multipleFunction2 = MultipleFunction();
            CHECK(multipleFunction.SequenceEqual(multipleFunction2));
        }
    }

    SECTION("With parameters by value")
    {
        SECTION("Single call")
        {
            CHECK(EmptyResult(0, 0.f).Count() == 0);
            CHECK(SingleFunction(9).First() == 9);
            CHECK(MultipleFunction(0.123f).Last() == 99.123f);
        }

        SECTION("Multiple calls")
        {
            for (int i = 0; i < 100; ++i)
            {
                CHECK(MultipleFunction(123.4f).Count() == 100);
                CHECK(MultipleFunction(12.3f).First() == 12.3f);
                CHECK(MultipleFunction(0.001f).Last() == 99.001f);
            }
        }

        SECTION("Single call stored")
        {
            auto emptyResult = EmptyResult(12, -4.5f);
            auto singleFunction = SingleFunction(20);
            auto multipleFunction = MultipleFunction(2.3f);

            CHECK(!emptyResult.Any());
            CHECK(singleFunction.First() == 20);
            CHECK(multipleFunction.Count() == 100);
        }

        SECTION("Equality")
        {
            CHECK(MultipleFunction(1234.456f).SequenceEqual(MultipleFunction(1234.456f)));

            auto multipleFunction = MultipleFunction(1234.456f);
            CHECK(multipleFunction.SequenceEqual(MultipleFunction(1234.456f)));

            multipleFunction = MultipleFunction(1234.456f);
            CHECK(MultipleFunction(1234.456f).SequenceEqual(multipleFunction));

            multipleFunction = MultipleFunction(1234.456f);
            auto multipleFunction2 = MultipleFunction(1234.456f);
            CHECK(multipleFunction.SequenceEqual(multipleFunction2));
        }
    }

    SECTION("Take")
    {
        CHECK(MultipleFunction().Take(1000).SequenceEqual(MultipleFunction()));

        CHECK(MultipleFunction().Skip(20).Take(40).First() == 20);
        CHECK(MultipleFunction().Skip(20).Take(40).ElementAt(20) == 40);
        CHECK(MultipleFunction().Skip(20).Take(40).Last() == 59);

        CHECK(MultipleFunction().Take(20).Concat(MultipleFunction().Skip(20)).SequenceEqual(MultipleFunction()));
    }

    SECTION("From")
    {
        CHECK(From(MultipleFunction()).SequenceEqual(MultipleFunction()));
        CHECK(From(MultipleFunction()).SequenceEqual(From(MultipleFunction())));

        CHECK(MultipleFunction().SequenceEqual(MultipleFunction()));
        CHECK(MultipleFunction().SequenceEqual(From(MultipleFunction())));
    }

    SECTION("Exception")
    {
        auto f1 = []
        {
            START_YIELDING(int)
            throw 3;
            END_YIELDING
        };

        auto ff = f1();

        CHECK_THROWS_AS(ff.Count(), int);
    }
}
