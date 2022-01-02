
#include "engine/utils/bind_this.h"

#include <catch2/catch.hpp>
#include <type_traits>

TEST_CASE("bind this") {
    struct test_t {
        const int x = 4;
        const int y = 3;
        int get_overloaded(bool x_or_y) { // NOLINT
            if (x_or_y) return x;
            return y;
        }

        int get_overloaded(int a) { // NOLINT
            if (a == 1) return y;
            return x;
        }
        int get_non_overloaded(int a) { // NOLINT
            if (a == 1) return y;
            return x;
        }
    };

    test_t test;

    STATIC_REQUIRE(std::is_same_v<decltype(binder(test))::type, test_t>);
    STATIC_REQUIRE(std::is_same_v<decltype(binder((const test_t)test))::type,
                                  const test_t>);

    auto this_binded = bind_this(test, &test_t::get_non_overloaded);
    auto this_binded_int = binder(test).bind<int, int>(&test_t::get_overloaded);
    auto this_binded_bool =
        binder(test).bind<int, bool>(&test_t::get_overloaded);

    REQUIRE(this_binded(1) == 3);
    REQUIRE(this_binded_int(1) == 3);
    REQUIRE(this_binded_bool(true) == 4);
}

TEST_CASE("bind this const"){

    struct test_t{
        int x=4;

        int get_x_const() const { // NOLINT
            return x;
        }
    };

    test_t test;


    auto this_binded_const = bind_this(test, &test_t::get_x_const);
    auto this_binded_const2 = binder(test).bind(&test_t::get_x_const);
    REQUIRE(this_binded_const() == 4);
    REQUIRE(this_binded_const2() == 4);

    const test_t test2;
    auto this_binded_const3 = bind_this(test2, &test_t::get_x_const);
    auto this_binded_const4 = binder(test2).bind(&test_t::get_x_const);
    REQUIRE(this_binded_const3() == 4);
    REQUIRE(this_binded_const4() == 4);
}


