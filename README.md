# Linqpp (LINQ for C++)

.NET devolopers are used to LINQ these days as it is very handy when working with enumerations. 
Since nothing comparable is available in standard C++, Linqpp tries to emulate the LINQ extension methods.

Linqpp is a header only library that has no other dependencies than STL (C++14).


## Getting started

```C++
#include "Enumeration.hpp" // the core of Linqpp that provides all functionality
#include <vector>
#include <list>
#include <iostream>

using namespace Linqpp;

int main()
{
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    double a[] = { 11.1, 12.2 };
    
    auto example = // frist, transfer v into a Linqpp enumeration using From()
        From(v) // anything for which std::begin(v) and std::end(v) is valid code can be used
        .Skip(2) // will return empty enumeration (and NOT fail) if too many elements are skipped
        .Take(5) // similar here
        .Where([](auto i) { return i % 2 != 0; }) // C++ lambdas are not as concise as in C# but still OK
        .Select([](auto i) { return i + 3.14; })
        .Concat(a); // again, whatever can deal with std::end and std::begin can be passed here
        
    // No computation has been done yet, lazy evaluation
        
    for (auto d : example) // Linqpp enumerations also work with std::begin and std::end, of course
        std::cout << d << " "; // output: 6.14 8.14 10.14 11.1 12.2
    std::cout << std::endl;
        
    if (example.Any())
        std::cout << "there are elements" << std::endl;
        
    std::cout << "actually, there are " << example.Count() << std::endl;
    
    if (!example.Any([](auto d) { return d < 6; }))
        std::cout << "but none is less than 6" << std::endl;
    
    std::list<int> l = { 1, 2, 3, 4, 5 };
    for (auto p : example.Zip(l, [](auto d, auto i) { return d * i; }).Take(2))
        std::cout << p << " "; // 6.14 16.28
}

```
