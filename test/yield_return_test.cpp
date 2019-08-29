#include <memory>
#include <vector>
#include <iostream>

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

        SECTION("Multiple calls stored")
        {
            auto multipleFunction = MultipleFunction();

            for (int i = 0; i < 100; ++i)
            {
                CHECK(multipleFunction.Count() == 100);
                CHECK(multipleFunction.First() == 0);
                CHECK(multipleFunction.Last() == 99);
            }
        }

        SECTION("Equality")
        {
            auto multipleFunction = MultipleFunction();

            CHECK(MultipleFunction().SequenceEqual(MultipleFunction()));
            CHECK(multipleFunction.SequenceEqual(MultipleFunction()));
            CHECK(MultipleFunction().SequenceEqual(multipleFunction));
            CHECK(multipleFunction.SequenceEqual(multipleFunction));
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

        SECTION("Multiple calls stored")
        {
            auto multipleFunction = MultipleFunction(0.1f);

            for (int i = 0; i < 100; ++i)
            {
                CHECK(multipleFunction.Count() == 100);
                CHECK(multipleFunction.First() == 0.1f);
                CHECK(multipleFunction.Last() == 99.1f);
            }
        }

        SECTION("Equality")
        {
            auto multipleFunction = MultipleFunction(1234.456f);

            CHECK(MultipleFunction(1234.456f).SequenceEqual(MultipleFunction(1234.456f)));
            CHECK(multipleFunction.SequenceEqual(MultipleFunction(1234.456f)));
            CHECK(MultipleFunction(1234.456f).SequenceEqual(multipleFunction));
            CHECK(multipleFunction.SequenceEqual(multipleFunction));
        }
    }

    SECTION("With parameters by lvalue reference")
    {
        SECTION("Single call")
        {
            int i = 9;
            float f = 0.123f;
            CHECK(EmptyResult(f).Count() == 0);
            CHECK(SingleFunction(f).First() == f);
            CHECK(MultipleFunction(i).Last() == 108);
        }

        SECTION("Multiple calls")
        {
            int a = 123;
            for (int i = 0; i < 100; ++i)
            {
                CHECK(MultipleFunction(a).Count() == 100);
                CHECK(MultipleFunction(a).First() == 123);
                CHECK(MultipleFunction(a).Last() == 222);
            }
        }

        SECTION("Single call stored")
        {
            int i = 12;
            float f = 2.3f;
            auto emptyResult = EmptyResult(f);
            auto singleFunction = SingleFunction(f);
            auto multipleFunction = MultipleFunction(i);

            CHECK(!emptyResult.Any());
            CHECK(singleFunction.First() == f);
            CHECK(multipleFunction.Count() == 100);
        }

        SECTION("Multiple calls stored")
        {
            int a = 2;
            auto multipleFunction = MultipleFunction(a);

            for (int i = 0; i < 100; ++i)
            {
                CHECK(multipleFunction.Count() == 100);
                CHECK(multipleFunction.First() == a);
                CHECK(multipleFunction.Last() == 99 + a);

                a += 12;
            }
        }

        SECTION("Equality")
        {
            int i = 1234;
            auto multipleFunction = MultipleFunction(i);

            CHECK(MultipleFunction(i).SequenceEqual(MultipleFunction(i)));
            i *= 2;
            CHECK(multipleFunction.SequenceEqual(MultipleFunction(i)));
            i *= 2;
            CHECK(MultipleFunction(i).SequenceEqual(multipleFunction));
            i *= 2;
            CHECK(multipleFunction.SequenceEqual(multipleFunction));
        }
    }

    SECTION("With parameters by const lvalue reference")
    {
        auto f = [](int const& i)
        {
            START_YIELDING(int)
            for (int j = 0; j < i; ++j)
                yield_return(j);
            END_YIELDING
        };

        CHECK(f(5).SequenceEqual(Enumerable::Range(0, 5)));
        CHECK(f(5).Select([](auto i) { return i; }).SequenceEqual(Enumerable::Range(0, 5)));
    }

    SECTION("With parameters by rvalue reference")
    {
        auto f = [](int&& i)
        {
            START_YIELDING(int)
            for (int j = 0; j < i; ++j)
                yield_return(j);
            END_YIELDING
        };

        CHECK(f(5).SequenceEqual(Enumerable::Range(0, 5)));
        CHECK(f(5).Select([](auto i) { return i; }).SequenceEqual(Enumerable::Range(0, 5)));
    }

    SECTION("With paramter by universal reverence")
    {
        auto f = [](auto&& i)
        {
            START_YIELDING(int)
            for (std::remove_const_t<std::remove_reference_t<decltype(i)>> j = 0; j < i; ++j)
                yield_return(j);
            END_YIELDING
        };

        CHECK(f(5).SequenceEqual(Enumerable::Range(0, 5)));
        CHECK(f(5).Select([](auto i) { return i; }).SequenceEqual(Enumerable::Range(0, 5)));

        CHECK(f(12.3).SequenceEqual(Enumerable::Range(0, 13)));

        const unsigned long l = 11;
        CHECK(f(l).SequenceEqual(Enumerable::Range(0, 11)));
    }

    SECTION("With non copyable parameters")
    {
        auto f = [](std::unique_ptr<int> spI)
        {
            START_YIELDING(int)
            for (int i = 0; i < *spI; ++i)
                yield_return(i);
            END_YIELDING
        };

        auto spI = std::make_unique<int>(10);
        CHECK(f(std::move(spI)).Count() == 10);

        auto ff = f(std::make_unique<int>(5));
        //CHECK(ff.Count() == 5);
        
        CHECK(f(std::make_unique<int>(5)).SequenceEqual(f(std::make_unique<int>(5))));
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
        auto m = MultipleFunction();

        CHECK(From(MultipleFunction()).SequenceEqual(MultipleFunction()));
        CHECK(From(MultipleFunction()).SequenceEqual(From(MultipleFunction())));
        CHECK(From(MultipleFunction()).SequenceEqual(m));
        CHECK(From(MultipleFunction()).SequenceEqual(From(m)));

        CHECK(m.SequenceEqual(MultipleFunction()));
        CHECK(m.SequenceEqual(From(MultipleFunction())));
        CHECK(m.SequenceEqual(m));
        CHECK(m.SequenceEqual(From(m)));

        CHECK(From(m).SequenceEqual(MultipleFunction()));
        CHECK(From(m).SequenceEqual(From(MultipleFunction())));
        CHECK(From(m).SequenceEqual(m));
        CHECK(From(m).SequenceEqual(From(m)));
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
