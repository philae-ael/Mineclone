#ifndef MAT_OPENGL_H_
#define MAT_OPENGL_H_

#include <cmath>
#include <numbers>

#include "mat.h"

namespace math {

template <typename T>
constexpr mat<T, 4, 4> translation(T x, T y, T z) {  // NOLINT
    auto res = identity<T, 4>();
    res[0][3] = x;
    res[1][3] = y;
    res[2][3] = z;
    return res;
}

template <typename T>
mat<T, 4, 4> projection(T aspect_ratio, T fov, T near, T far) {  // NOLINT
    const float f =
        static_cast<float>(std::tan(std::numbers::pi / 2 - fov / 2));
    const auto A = (near + far) / (near - far);
    const auto B = 2 * near * far / (near - far);
    return {{{f / aspect_ratio, 0, 0, 0},
             {0, f, 0, 0},
             {0, 0, A, B},
             {0, 0, -1, 0}}};
}

template <typename T>
constexpr mat<T, 4, 4> scale(T s) {  // NOLINT
    return diag<T>(s, s, s, 1);
}

constexpr math::mat4f lookAt(math::vec3f relative_position) {
    auto v = math::normalize(relative_position);

    // the aim is to enode a rotation that maps (0, 0, -1) onto
    // v; Or (0,0,1) onto -v.
    // We can achieve this by composing only two rotation, one along the x
    // axis and one along the y axix
    const math::vec3f x{1, 0, 0}, y{0, 1, 0}, z{0, 0, 1};
    v = -v;  // now we map (0, 0, 1) onto v

    // y rot
    auto v_proj_y = math::normalize(v - math::dot(v, y) * y);
    if (math::norm2(v_proj_y) == 0) v_proj_y = z;
    const auto v_perp_y = math::normalize(math::wedge(y, v_proj_y));

    const math::mat3f rot_y{{
        {v_perp_y[0], 0, v_perp_y[2]},
        {0, 1, 0},
        {v_proj_y[0], 0, v_proj_y[2]},
    }};

    // Change v of base
    const auto v2 = rot_y * v;
    // v2 should be on the (Oyz) plane, no need to project

    // x rot
    auto v_proj_x = math::normalize(v2);
    if (math::norm2(v_proj_x) == 0) v_proj_x = z;
    const auto v_perp_x = math::normalize(math::wedge(v_proj_x, x));

    const math::mat3f rot_x{{
        {1, 0, 0},
        {0, v_perp_x[1], v_perp_x[2]},
        {0, v_proj_x[1], v_proj_x[2]},
    }};

    return math::extend<float, 3, 3, 4>(rot_x * rot_y);
}

};      // namespace math
#endif  // !MAT_OPENGL_H
