#include "engine/utils/math.h"

#include <catch2/catch.hpp>
#include <iostream>

using math::vec3i;

TEST_CASE("vec basic operations") {
    std::cout << sizeof(vec3i) << "\n";

    STATIC_REQUIRE(vec3i{0, 0, 0} == vec3i{});
    STATIC_REQUIRE(vec3i{1, 2, 3} + vec3i{3, 2, 1} == vec3i{4, 4, 4});
    STATIC_REQUIRE(vec3i{1, 2, 3} - vec3i{3, 2, 1} == vec3i{-2, 0, 2});
    STATIC_REQUIRE(vec3i{1, 2, 3} * vec3i{3, 2, 1} == vec3i{3, 4, 3});
    STATIC_REQUIRE(vec3i{4, 6, 2} / vec3i{2, 2, 1} == vec3i{2, 3, 2});
    STATIC_REQUIRE(-vec3i{1, 2, 1} == vec3i{-1, -2, -1});

}

TEST_CASE("vec norm, wedge, norm2") {
    STATIC_REQUIRE(math::wedge(vec3i{1, 0, 0}, vec3i{0, 1, 0}) ==
                   vec3i{0, 0, 1});
    STATIC_REQUIRE(math::wedge(vec3i{0, 1, 0}, vec3i{1, 0, 0}) ==
                   vec3i{0, 0, -1});
    STATIC_REQUIRE(math::wedge(vec3i{0, 1, 0}, vec3i{0, 0, 1}) ==
                   vec3i{1, 0, 0});

    STATIC_REQUIRE(math::dot(vec3i{1, 0, 0}, vec3i{0, 1, 0}) == 0);
    STATIC_REQUIRE(vec3i{1, 2, 1}.norm2() == 6);

}

TEST_CASE("vec +=/-=/...") {
    auto l1 = []() constexpr {
        vec3i v{0, 0, 0};
        v += vec3i{1, 2, 3};
        return v;
    };
    STATIC_REQUIRE(l1() == vec3i{1, 2, 3});

    auto l2 = []() constexpr {
        vec3i v{0, 0, 0};
        v -= vec3i{1, 2, 3};
        return v;
    };
    STATIC_REQUIRE(l2() == vec3i{-1, -2, -3});
    auto l3 = []() constexpr {
        vec3i v{1, 0, 3};
        v *= vec3i{1, 2, 3};
        return v;
    };
    STATIC_REQUIRE(l3() == vec3i{1, 0, 9});
    auto l4 = []() constexpr {
        vec3i v{2, 3, 4};
        v /= vec3i{2, 1, 1};
        return v;
    };
    STATIC_REQUIRE(l4() == vec3i{1, 3, 4});
}

TEST_CASE("vec scalar `op` vector") {
    STATIC_REQUIRE(3*vec3i{1, 2, 3} == vec3i{3, 6, 9});
    STATIC_REQUIRE(3+vec3i{1, 2, 3} == vec3i{4, 5, 6});
}
