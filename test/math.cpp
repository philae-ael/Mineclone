#include "engine/utils/math.h"

#include <catch2/catch.hpp>
#include <cmath>
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
        mat2i m{{{1, 2}, {3, 4}}};
        m += mat2i{{{1, 2}, {3, 4}}};
        return m;
    };
    STATIC_REQUIRE(l1() == mat2i{{{2, 4}, {6, 8}}});

    auto l2 = []() constexpr {
        mat2i m{{{1, 2}, {3, 4}}};
        m -= mat2i{{{1, 2}, {3, 4}}};
        return m;
    };
    STATIC_REQUIRE(l2() == mat2i{});

    auto l3 = []() constexpr {
        mat2i m{{{1, 2}, {3, 4}}};
        m /= mat2i{{{1, 2}, {3, 4}}};
        return m;
    };
    STATIC_REQUIRE(l3() == mat2i{{{1, 1}, {1, 1}}});
    auto l4 = []() constexpr {
        mat2i m{{{1, 2}, {3, 4}}};
        m *= mat2i{{{1, 2}, {3, 4}}};
        return m;
    };
    STATIC_REQUIRE(l4() == mat2i{{{1, 4}, {9, 16}}});
}
TEST_CASE("math::mat +=/-=/...") {
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

TEST_CASE("math::mat scalar `op` mat") {
    STATIC_REQUIRE(3 * mat2i{{{1, 2}, {3, 4}}} == mat2i{{{3, 6}, {9, 12}}});
    STATIC_REQUIRE(3 + mat2i{{{1, 2}, {3, 4}}} == mat2i{{{4, 5}, {6, 7}}});
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

TEST_CASE("math near") {
    constexpr double eps = 1e-1;
    STATIC_REQUIRE(
        math::near(math::vec2d{-2., 1.}, math::vec2d{-2., 1.}, eps));  // NOLINT
    STATIC_REQUIRE_FALSE(math::near(math::vec2d{-2. + eps, 1.},
                                    math::vec2d{-2., 1.}, eps));  // NOLINT
}
TEST_CASE("math rotate") {
    constexpr double eps = 1e-1;
    constexpr math::vec2d v1{1, 2};
    constexpr math::mat2d rot = math::rotate<double>(M_PI / 2);

    constexpr auto res = math::dot(rot, v1);

    STATIC_REQUIRE(math::near(res, math::vec2d{2., -1.}, eps));  // NOLINT

    constexpr math::vec3d v2{1, 0, 0};
    constexpr math::mat3d rot2 = math::rotate(M_PI / 2, v2);

    constexpr auto res2 = math::dot(rot2, v2);

    STATIC_REQUIRE(math::near(res2, v2, eps));  // NOLINT
}

TEST_CASE("math constexpr") {
    auto abs = [](auto x) constexpr->decltype(x) {
        if (x < 0) return -x;
        return x;
    };
    constexpr double eps = 1e-1;
    STATIC_REQUIRE(abs(math::cos(M_PI) + 1) < eps);  // NOLINT
    STATIC_REQUIRE(abs(math::sqrt(4) - 2) < eps);    // NOLINT
    STATIC_REQUIRE(abs(math::sqrt(1) - 1) < eps);    // NOLINT
}
