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
mat<T, 4, 4> projection(T aspect_ratio, T fov, T near, T far) {  // NOLINT
    const float f = std::tan(M_PI / 2 - fov / 2);
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

};      // namespace math
#endif  // !MAT_OPENGL_H
