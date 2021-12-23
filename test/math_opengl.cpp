#include "engine/utils/mat.h"
#include "engine/utils/mat_opengl.h"

#include <catch2/catch.hpp>

using math::mat2i;
using math::vec3i;


TEST_CASE("math::mat translate") {
    constexpr auto trl = math::translation<float>(1, 0, 3);

    STATIC_REQUIRE(trl * math::vec4f{0, 0, 0, 1} == math::vec4f{1, 0, 3, 1});
}
