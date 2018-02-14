#include <forward_list>
#include <list>
#include <string>
#include <type_traits>
#include <vector>

#include "Linqpp.hpp"
#include "catch.hpp"

struct Int
{
    int _i;
    Int(int i) : _i(i) { }
    Int() = default;
    Int(Int const&) = default;
    Int(Int&&) = default;
    Int& operator=(Int&&) = default;

    Int& operator=(Int const&) = delete;

    bool operator<(Int ii) const { return _i < ii._i; }
    bool operator>(Int ii) const { return _i > ii._i; }
    bool operator==(Int ii) const { return _i == ii._i; }
    Int operator+(Int ii) const { return Int(_i + ii._i); }
    Int operator-() const { return Int(-_i); }
    explicit operator int() const { return _i; }
};

namespace std
{
    template <>
    struct hash<Int>
    {
        auto operator()(Int i) const { return hash<int>()(i._i); }
    };
}

std::string to_string(Int i)
{
    return std::to_string(i._i);
}

typedef Int test_t;

static_assert(!std::is_copy_assignable<test_t>::value, "test_t is copy assignable");
static_assert(std::is_move_assignable<test_t>::value, "test_t is not move assignable");

TEST_CASE("move assignable test")
{
    #include "test_template.hpp"
}
