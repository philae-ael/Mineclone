#ifndef MAT_H_
#define MAT_H_

#include <algorithm>
#include <concepts>
#include <functional>
#include <numeric>

#include "constexpr_functions.h"
namespace math {

template <typename T, int N, int M>
struct mat {
   public:
   private:
    // This structs allow to make vec and mat only one class while having
    // different kinds of accessors
    template <int M_>
    struct details {
        using value_type = mat<T, M_, 1>;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using iterator = value_type *;
        using const_iterator = const value_type *;

        using storage_kind = value_type[N];  // NOLINT
        constexpr static typename value_type::const_reference get(
            const storage_kind &data, int i, int j) {
            return data[i].at(j);
        }
        constexpr static typename value_type::reference get(storage_kind &data,
                                                            int i, int j) {
            return data[i].at(j);
        }
    };

    template <>
    struct details<1> {
        using value_type = T;
        using reference = value_type &;
        using const_reference = const value_type &;
        using pointer = value_type *;
        using iterator = value_type *;
        using const_iterator = const value_type *;

        using storage_kind = value_type[N];  // NOLINT
        constexpr static const_reference get(const storage_kind &data, int i,
                                             int /*j*/) {
            return data[i];
        }
        constexpr static reference get(storage_kind &data, int i, int /*j*/) {
            return data[i];
        }
    };

   public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;

    using value_stored = typename details<M>::value_type;
    using reference_stored = typename details<M>::reference;
    using const_reference_stored = typename details<M>::const_reference;
    using pointer_stored = typename details<M>::pointer;

    using iterator = typename details<M>::iterator;
    using const_iterator = typename details<M>::const_iterator;

    typename details<M>::storage_kind
        data;  // NOLINT: this is intended to be public: this struct meant to be
               // a POD

    constexpr reference_stored operator[](int i) { return at(i); }
    constexpr const_reference_stored operator[](int i) const { return at(i); }

    constexpr reference_stored at(int i) { return data[i]; }
    constexpr const_reference_stored at(int i) const { return data[i]; }

    constexpr reference at(int i, int j) { return details<M>::get(data, i, j); }
    constexpr const_reference at(int i, int j) const {
        return details<M>::get(data, i, j);
    }

    constexpr iterator begin() { return iterator(data); }
    constexpr iterator end() { return iterator(data + N); }
    constexpr const_iterator cbegin() const { return const_iterator(data); }
    constexpr const_iterator cend() const { return const_iterator(data + N); }

    pointer ptr() { return &details<M>::get(data, 0, 0); }

    constexpr mat &operator-() {
        std::transform(this->cbegin(), this->cend(), this->begin(),
                       [](auto x) { return -x; });
        return *this;
    }

    constexpr friend bool operator==(const mat &lhs, const mat &rhs) {
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    constexpr mat &operator+=(const mat &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::plus{});
        return *this;
    }

    constexpr mat &operator-=(const mat &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::minus{});
        return *this;
    }

    constexpr mat &operator*=(const mat &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::multiplies{});
        return *this;
    }

    constexpr mat &operator/=(const mat &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::divides{});
        return *this;
    }

    constexpr mat friend operator*(const mat &lhs, const mat &rhs) {
        mat res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::multiplies{});
        return res;
    }

    constexpr mat friend operator/(const mat &lhs, const mat &rhs) {
        mat res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::divides{});
        return res;
    }

    constexpr mat friend operator+(const mat &lhs, const mat &rhs) {
        mat res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::plus{});
        return res;
    }

    constexpr mat friend operator-(const mat &lhs, const mat &rhs) {
        mat res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::minus{});
        return res;
    }

    //
    // Operation with a scalar : v2 = scalar `op` v
    // Note: There is no operator/, operator- (because i want the usual order
    // scalar `op` to be respected), use resp. (1/x)*v, (-x)*v
    //
    constexpr mat friend operator*(const value_type scalar, const mat &rhs) {
        mat res;
        std::transform(rhs.cbegin(), rhs.cend(), res.begin(),
                       [scalar](const auto &rhs_) { return scalar * rhs_; });
        return res;
    }
    constexpr mat friend operator+(const value_type scalar, const mat &rhs) {
        mat res;
        std::transform(rhs.cbegin(), rhs.cend(), res.begin(),
                       [scalar](const auto &rhs_) { return scalar + rhs_; });
        return res;
    }

    template <int L>
    // I have no other choice of operator because of precedences rules and bc i
    // already use +-*/ At least % respects classic precedences rules with +-
    // (but NOT with * /) and is left associative
    constexpr friend mat<T, N, L> operator%(const mat<T, N, M> &rhs,
                                            const mat<T, M, L> &lhs) {
        return dot<T, N, M, L>(rhs, lhs);
    }
};

template <typename T, int N>
using vec = mat<T, N, 1>;

template <typename T>
constexpr auto diag(const T d1, std::convertible_to<const T> auto... args)
    -> mat<T, 1 + sizeof...(args), 1 + sizeof...(args)> {
    const unsigned int N = 1 + sizeof...(args);

    std::array<T, N> argv{d1, static_cast<const T>(args)...};
    mat<T, N, N> res{};

    for (unsigned int i = 0; i < 1 + sizeof...(args); i++) {
        res[i][i] = argv[i];
    }

    return res;
}

template <typename T, int N>
constexpr mat<T, N, N> identity() {
    mat<T, N, N> res{};
    for (int i = 0; i < N; i++) {
        res.at(i, i) = 1;
    }
    return res;
}

template <typename T>
constexpr mat<T, 4, 4> translation(T x, T y, T z) {  // NOLINT
    auto res = identity<T, 4>();
    res[0][3] = x;
    res[1][3] = y;
    res[2][3] = z;
    return res;
}

template <typename T>
constexpr mat<T, 4, 4> frustrum(T near, T far, T left, T right, T bottom, // NOLINT
                                  T top) {
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
mat<T, 4, 4> projection(T aspect_ratio, T fov, T near, T far) { // NOLINT
    const auto top = std::tan(fov/2) * far;
    const auto bottom = -top;
    const auto right = top*aspect_ratio;
    const auto left = -right;

    // Note that object of radius up to z*right/(2 near) will be drawn 
    // With fov of pi/2, object of size up to z*far/(2*near) 
    return frustrum(near, far, left, right, bottom, top);
}

template <typename T>
constexpr mat<T, 4, 4> scale(T s) { // NOLINT
    return diag<T>(s, s, s, 1);
}


template <typename T, int N>
constexpr vec<T, N> normalize(const vec<T, N> &v) {
    auto n = norm2(v);
    if (n == 0.)
        return v;  // v == 0 0 0...  so it doesnt really matter allow constexpr
                   // evaluation
    return sqrt(1 / n) * v;
}

template <typename T>
constexpr mat<T, 2, 2> rotate(double angle) {
    return {{{static_cast<T>(cos(angle)), static_cast<T>(sin(angle))},
             {static_cast<T>(-sin(angle)), static_cast<T>(cos(angle))}}};
}

template <typename T, int L, int M, int N>
constexpr mat<T, N, N> extend(mat<T, L, M> m) {
    auto res = identity<T, N>();
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < L; j++) {
            res.at(i, j) = m.at(i, j);
        }
    }
    return res;
}

template <typename T, int N>
constexpr mat<T, N, N> rotate(double angle, vec<T, 3> axis) {
    static_assert(N >= 3);

    using vec3T = vec<T, 3>;
    axis = normalize(axis);

    // either 1, 0, 0 or 0, 1, 0 is not parallel to p1, thus we can find a
    // perpendicular vector to axis
    vec3T p1 = wedge(axis, {1, 0, 0});
    p1 = normalize(p1);
    if (norm2(p1) <
        0.1) {  // NOLINT: if this is verified, axis is parallel to 1 0 0
        p1 = wedge(axis, {0, 1, 0});
        p1 = normalize(p1);
    }

    auto p2 = wedge(axis, p1);

    // should be automatic because axis and p1 are perpendicular and
    // normalized thus axis `wedge` p1 has norm 1
    p2 = normalize(p2);

    mat<T, 3, 3> rot3 = extend<T, 2, 2, 3>(rotate<T>(angle));
    mat<T, 3, 3> P_inv =
        mat<T, 3, 3>{p1, p2, axis};  // Care, p1 will be first row and so on
    mat<T, 3, 3> P = transpose(P_inv);

    return extend<T, 3, 3, N>(dot(dot(P, rot3), P_inv));
}

template <typename T, int N, int M>
constexpr mat<T, M, N> transpose(const mat<T, N, M> &m) {
    mat<T, M, N> res;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < M; j++) {
            res.at(j, i) = m.at(i, j);
        }
    }
    return res;
}

// Quite a naive implementation, hopefully, should be optimized out by compiler
// for small values of L, M, N
template <typename T, int L, int M, int N>
constexpr mat<T, L, N> dot(const mat<T, L, M> &lhs, const mat<T, M, N> &rhs) {
    mat<T, L, N> res;
    mat<T, N, M> rhsT = transpose(rhs);  // should allow better cache locality
    for (int i = 0; i < L; i++) {
        for (int j = 0; j < N; j++) {
            res.at(i, j) = dot(mat{lhs.at(i)}, mat{rhsT.at(j)});
        }
    }
    return res;
}

// vector specific functions

template <typename T, int N>
constexpr typename vec<T, N>::value_type dot(const vec<T, N> &lhs,
                                             const vec<T, N> &rhs) {
    const mat tmp = lhs * rhs;
    return std::accumulate(tmp.cbegin(), tmp.cend(),
                           typename vec<T, N>::value_type{});
}

template <typename T, int N>
constexpr typename vec<T, N>::value_type norm2(const vec<T, N> &v) {
    return dot(v, v);
}

template <typename T, int N>
constexpr bool near(const vec<T, N> &v1, const vec<T, N> &v2, T eps) {
    return norm2(v2 - v1) < eps * eps;
}

template <typename T>
constexpr vec<T, 3> wedge(const vec<T, 3> &lhs, const vec<T, 3> &rhs) {
    return {lhs.data[1] * rhs.data[2] - lhs.data[2] * rhs.data[1],
            lhs.data[2] * rhs.data[0] - lhs.data[0] * rhs.data[2],
            lhs.data[0] * rhs.data[1] - lhs.data[1] * rhs.data[0]};
};

using vec2f = vec<float, 2>;
using mat2f = mat<float, 2, 2>;
using vec3f = vec<float, 3>;
using mat3f = mat<float, 3, 3>;
using vec4f = vec<float, 4>;
using mat4f = mat<float, 4, 4>;

using vec2d = vec<double, 2>;
using mat2d = mat<double, 2, 2>;
using vec3d = vec<double, 3>;
using mat3d = mat<double, 3, 3>;
using vec4d = vec<double, 4>;
using mat4d = mat<double, 4, 4>;

using vec2i = vec<int, 2>;
using mat2i = mat<int, 2, 2>;
using vec3i = vec<int, 3>;
using mat3i = mat<int, 3, 3>;
using vec4i = vec<int, 4>;
using mat4i = mat<int, 4, 4>;

}  // namespace math

#endif  // !MAT_H_
