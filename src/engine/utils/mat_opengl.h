#ifndef MAT_OPENGL_H_
#define MAT_OPENGL_H_

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
constexpr mat<T, 4, 4> frustrum(T near, T far, T left, T right,  // NOLINT
                                T bottom, T top) {
    // Let's remember that OpenGL will do two things
    // - OpenGL clips all vertices such that
    //      abs(x) < w, abs(y) < w or abs(z) < w
    // - THEN OpenGL normalize w to 1
    // Furthermore, remember that camera looks towards the -Oz direction
    // Because we want perspective, let's put -z into w
    // We want OpenGL to clip every z between near and far thus z should be
    // mapped from near to far to R such that -near -> -near, -far-> far
    // thus the 2 equations $-A*near + B = -near$ and $-A*far + B = far$
    // We find $A = (n+f)/(n-f)$, $B = 2fn/(n-f)$
    // Note that if f=infty, n = 0, u have A = 1, B = 0 !
    //
    // Then we want to project (x, y, z) onto the near plane
    // why? it should works no matter what is whosen as projection plane
    // because of the normalization, z -> 1 thus, if we wanna be linear,
    // just multiply by n
    //
    // then we want to map x from l, r to -1, 1, easy
    // x -> 2*(x - l)/(r - l) - 1 = 2x/(r+l) + (r + l)/(r-l)
    // then we want to map y from b, t to -1, 1, easy
    // x -> 2*(x - b)/(t - b) - 1 = 2x/(b+t) + (b + t)/(t-b)
    // just remember that we are divided by z afterwards
    const auto A = (near + far) / (near - far);
    const auto B = 2 * near * far / (near - far);
    const auto res = mat<T, 4, 4>{{
        {2 * near / (right - left), 0, (right + left) / (right - left), 0},
        {0, 2 * near / (top - bottom), (bottom + top) / (top - bottom), 0},
        {0, 0, A, B},
        {0, 0, -1, 0},
    }};
    return res;
}

template <typename T>
mat<T, 4, 4> projection(T aspect_ratio, T fov, T near, T far) {  // NOLINT
    const auto top = std::tan(fov / 2) * far;
    const auto bottom = -top;
    const auto right = top * aspect_ratio;
    const auto left = -right;

    // Note that object of radius up to z*right/(2 near) will be drawn
    // With fov of pi/2, object of size up to z*far/(2*near)
    return frustrum(near, far, left, right, bottom, top);
}

template <typename T>
constexpr mat<T, 4, 4> scale(T s) {  // NOLINT
    return diag<T>(s, s, s, 1);
}

};      // namespace math
#endif  // !MAT_OPENGL_H
