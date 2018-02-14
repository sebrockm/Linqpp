#include <forward_list>
#include <list>
#include <type_traits>
#include <vector>

#include "Linqpp.hpp"
#include "catch.hpp"

typedef int test_t;

static_assert(std::is_copy_assignable<test_t>::value, "test_t is not copy assignable");
static_assert(std::is_move_assignable<test_t>::value, "test_t is not move assignable");

TEST_CASE("copy assignable test")
{
    #include "test_template.hpp"
}
