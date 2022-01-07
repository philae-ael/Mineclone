#include <array>
#include <catch2/catch.hpp>

#include "engine/utils/logging.h"
#include "engine/utils/range_iterator.h"

TEST_CASE("range_it 1d") {
    range_it<unsigned int, 5> r;
    REQUIRE((*r)[0] == 0);
    r++;
    REQUIRE((*r)[0] == 1);
    r++;
    REQUIRE((*r)[0] == 2);
    r++;
    REQUIRE((*r)[0] == 3);
    r++;
    REQUIRE((*r)[0] == 4);
    r++;
    REQUIRE(r == r.end());
}

TEST_CASE("range_it 1d for") {
    range_it<unsigned int, 5> r;
    std::array<unsigned int, 5> a{};
    for (auto [i] : r) {
        a[i] = i;
    }
    REQUIRE(a == std::array<unsigned int, 5>{0, 1, 2, 3, 4});
}

TEST_CASE("range_it 2d for") {
    range_it<unsigned int, 3, 2> r;
    std::array<std::array<unsigned int, 2>, 6> a{};
    for (auto [i, j] : r) {
        a[i + j * 3] = std::array<unsigned int, 2>{i, j};
    }
    REQUIRE(a == std::array<std::array<unsigned int, 2>, 6>{{
                     {0, 0},
                     {1, 0},
                     {2, 0},
                     {0, 1},
                     {1, 1},
                     {2, 1},
                 }});
}

TEST_CASE("range_it 3d for") {
    range_it<unsigned int, 2, 2, 2> r;
    std::array<std::array<unsigned int, 3>, 8> a{};
    for (auto [i, j, k] : r) {
        a[i + j * 2 + k * 4] = std::array<unsigned int, 3>{i, j, k};
    }
    REQUIRE(a == std::array<std::array<unsigned int, 3>, 8>{{
                     {0, 0, 0},
                     {1, 0, 0},
                     {0, 1, 0},
                     {1, 1, 0},
                     {0, 0, 1},
                     {1, 0, 1},
                     {0, 1, 1},
                     {1, 1, 1},
                 }});
}
