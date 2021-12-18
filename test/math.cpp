#include "engine/utils/math.h"

#include <catch2/catch.hpp>
#include <iostream>

using math::mat2i;
using math::vec3i;

TEST_CASE("math::vec basic operations") {
    STATIC_REQUIRE(vec3i{0, 0, 0} == vec3i{});
    STATIC_REQUIRE(vec3i{1, 2, 3} + vec3i{3, 2, 1} == vec3i{4, 4, 4});
    STATIC_REQUIRE(vec3i{1, 2, 3} - vec3i{3, 2, 1} == vec3i{-2, 0, 2});
    STATIC_REQUIRE(vec3i{1, 2, 3} * vec3i{3, 2, 1} == vec3i{3, 4, 3});
    STATIC_REQUIRE(vec3i{4, 6, 2} / vec3i{2, 2, 1} == vec3i{2, 3, 2});
    STATIC_REQUIRE(-vec3i{1, 2, 1} == vec3i{-1, -2, -1});
}

TEST_CASE("math::mat basic operations") {
    STATIC_REQUIRE(mat2i{{{0, 0}, {0, 0}}} == mat2i{});
    STATIC_REQUIRE(mat2i{{{0, 0}, {-1, -2}}} + mat2i{{{1, 2}, {2, 1}}} ==
                   mat2i{{{1, 2}, {1, -1}}});
    STATIC_REQUIRE(mat2i{{{0, 0}, {-1, -2}}} - mat2i{{{1, 2}, {2, 1}}} ==
                   mat2i{{{-1, -2}, {-3, -3}}});
    STATIC_REQUIRE(mat2i{{{0, 0}, {-1, -2}}} * mat2i{{{1, 2}, {2, 1}}} ==
                   mat2i{{{0, 0}, {-2, -2}}});
    STATIC_REQUIRE(mat2i{{{0, 0}, {1, 2}}} / mat2i{{{1, 2}, {1, 1}}} ==
                   mat2i{{{0, 0}, {1, 2}}});
    STATIC_REQUIRE(-mat2i{{{0, 0}, {-1, -2}}} == mat2i{{{0, 0}, {1, 2}}});
}

TEST_CASE("math::vec norm, wedge, norm2") {
    STATIC_REQUIRE(math::wedge(vec3i{1, 0, 0}, vec3i{0, 1, 0}) ==
                   vec3i{0, 0, 1});
    STATIC_REQUIRE(math::wedge(vec3i{0, 1, 0}, vec3i{1, 0, 0}) ==
                   vec3i{0, 0, -1});
    STATIC_REQUIRE(math::wedge(vec3i{0, 1, 0}, vec3i{0, 0, 1}) ==
                   vec3i{1, 0, 0});

    STATIC_REQUIRE(math::dot(vec3i{1, 0, 0}, vec3i{0, 1, 0}) == 0);
    STATIC_REQUIRE(math::norm2(vec3i{1, 2, 1}) == 6);
}

TEST_CASE("math::vec +=/-=/...") {
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

TEST_CASE("math::vec scalar `op` vector") {
    STATIC_REQUIRE(3 * vec3i{1, 2, 3} == vec3i{3, 6, 9});
    STATIC_REQUIRE(3 + vec3i{1, 2, 3} == vec3i{4, 5, 6});
}

TEST_CASE("math::mat data access") {
    STATIC_REQUIRE(vec3i{1, 2, 3}.at(1) == 2);
    STATIC_REQUIRE(vec3i{1, 2, 3}.at(1, 0) == 2);
    STATIC_REQUIRE(mat2i{{{2, 3}, {2, 3}}}.at(1, 0) == 2);
}

TEST_CASE("math::mat transpose") {
    STATIC_REQUIRE(math::transpose(math::vec2i{1, 2}) ==
                   math::mat<int, 1, 2>{{{1, 2}}});
}

TEST_CASE("math::mat dot") {
    constexpr math::mat<int, 1, 2> m1{{{1, 2}}};
    constexpr math::mat<int, 2, 1> m2{1, 2};

    STATIC_REQUIRE(math::dot(m1, m2) == math::mat<int, 1, 1>{5});

    constexpr math::mat<int, 2, 2> m3{{{0, 1}, {1, 0}}};
    constexpr math::mat<int, 2, 3> m4{{{1, 2, 0}, {1, 2, 3}}};

    STATIC_REQUIRE(math::dot(m1, m2) == math::mat<int, 1, 1>{5});
    STATIC_REQUIRE(math::dot(m3, m4) ==
                   math::mat<int, 2, 3>{{{1, 2, 3}, {1, 2, 0}}});

    constexpr math::mat<int, 2, 2> m5{{{1, 0}, {0, -1}}};
    constexpr math::vec2i v{1, 2};
    STATIC_REQUIRE(math::dot(m5, v) == math::vec2i{1, -2});
}
