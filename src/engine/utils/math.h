#ifndef MATH_H_
#define MATH_H_

#include <unistd.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <numeric>
#include <type_traits>
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
};

template <typename T, int N>
using vec = mat<T, N, 1>;

template <typename T, int N>
constexpr mat<T, N, N> identity() {
    mat<T, N, N> res{};
    for (int i = 0; i < N; i++) {
        res.at(i, i) = 1;
    }
    return res;
}

constexpr double cos(double angle) {
    if (std::is_constant_evaluated()) {
        const unsigned int max_iter = 100;

        auto _cos = [](double angle) {
            double anglepn = 1, acc = 0, fac = 1;
            for (unsigned int i = 0; i < max_iter;) {
                acc += anglepn / fac;
                fac *= (i + 1) * (i + 2);
                anglepn *= -angle * angle;
                i = i + 2;
            }
            return acc;
        };

        return _cos(angle);
    }
    return std::cos(angle);
}

constexpr double sin(double angle) {
    if (std::is_constant_evaluated()) {
        // Should be enough, lol
        const unsigned int max_iter = 100;

        auto _sin = [](double angle) {
            double anglepn = angle, acc = 0, fac = 1;
            for (unsigned int i = 1; i < max_iter;) {
                acc += anglepn / fac;
                fac *= (i + 1) * (i + 2);
                anglepn *= -angle * angle;
                i = i + 2;
            }
            return acc;
        };

        return _sin(angle);
    }
    return std::sin(angle);
}

constexpr double sqrt(const double x) {
    if (std::is_constant_evaluated()) {
        const unsigned int max_iter = 100;
        const double eps = 1e-4;
        double y = x;
        for (unsigned int i = 1; i < max_iter; i++) {
            if (y * y - x < eps) return y;
            y = y - (y * y - x) / (2 * y);
        }
        return y;
    }
    return std::sqrt(x);
}

template <typename T, int N>
constexpr vec<T, N> normalize(const vec<T, N> &v) {
    auto n = norm2(v);
    if (n == 0.) return v;  // v == 0 0 0...  so it doesnt really matter
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

template <typename T>
constexpr mat<T, 3, 3> rotate(double angle, vec<T, 3> axis) {
    using vec3T = vec<T, 3>;
    axis = normalize(axis);

    // either 1, 0, 0 or 0, 1, 0 is not parallel to p1, thus we can find a
    // perpendicular vector to axis
    vec3T p1 = wedge(axis, {1, 0, 0});
    p1 = normalize(p1);
    if (norm2(p1) <
        0.1) {  // NOLINT: if this is verified, axis is parallel to axis
        p1 = wedge(axis, {0, 1, 0});
        p1 = normalize(p1);
    }

    auto p2 = wedge(axis, p1);

    // should be automatic because axis and p1 are perpendicular and
    // normalized thus axis `wedge` p1 has norm 1
    p2 = normalize(p2);

    mat<T, 3, 3> rot3 = extend<T, 2, 2, 3>(rotate<T>(angle));
    mat<T, 3, 3> P = mat<T, 3, 3>{p1, p2, axis};
    mat<T, 3, 3> P_inv = transpose(P);

    return dot(dot(P, rot3), P_inv);
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
#endif  // !MATH_H_
