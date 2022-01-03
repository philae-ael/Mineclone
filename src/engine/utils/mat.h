#ifndef MAT_H_
#define MAT_H_

#include <algorithm>
#include <concepts>
#include <functional>
#include <iomanip>
#include <numeric>
#include <ostream>
#include <type_traits>

#include "constexpr_functions.h"
namespace math {

template <typename T, int N, int M,
          typename = typename std::enable_if_t<std::is_arithmetic_v<T>>>
struct mat {
   public:
   private:
    // This structs allows to make vec and mat only one class while having
    // different kinds of accessors
    template <int M_, class dummy = int>  // the dummy is needed because gcc
                                          // does not implement CWG 727
    struct details_ {
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

    template <class dummy>
    struct details_<1, dummy> {
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

    using details = details_<M>;

   public:
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;

    using value_stored = typename details::value_type;
    using reference_stored = typename details::reference;
    using const_reference_stored = typename details::const_reference;
    using pointer_stored = typename details::pointer;

    using iterator = typename details::iterator;
    using const_iterator = typename details::const_iterator;

    typename details::storage_kind data;

    constexpr reference_stored operator[](int i) { return at(i); }
    constexpr const_reference_stored operator[](int i) const { return at(i); }

    constexpr reference_stored at(int i) { return data[i]; }
    constexpr const_reference_stored at(int i) const { return data[i]; }

    constexpr reference at(int i, int j) { return details::get(data, i, j); }
    constexpr const_reference at(int i, int j) const {
        return details::get(data, i, j);
    }
    // NOLINT
    constexpr iterator begin() { return iterator(data); }
    constexpr iterator end() { return iterator(data + N); }
    constexpr const_iterator begin() const { return const_iterator(data); }
    constexpr const_iterator end() const { return const_iterator(data + N); }
    constexpr const_iterator cbegin() const { return const_iterator(data); }
    constexpr const_iterator cend() const { return const_iterator(data + N); }

    pointer ptr() { return &details::get(data, 0, 0); }
};

template <typename T, int N, int M>
std::ostream &operator<<(std::ostream &s, const math::mat<T, N, M> &m) {
    if (M != 1) s << "\n";

    // NOTE: this is recursive, it works because auto is vec is M > 1
    // and mat::value_type otherwise
    for (const auto &c : m) {
        if (M != 1) s << "    ";
        s << std::setw(6) << std::setprecision(3) << c << " ";
        if (M != 1) s << "\n";
    }

    return s;
}

template <typename T, int N, int M>
constexpr mat<T, N, M> operator-(const mat<T, N, M> &rhs) {
    mat<T, N, M> res;
    std::transform(rhs.cbegin(), rhs.cend(), res.begin(),
                   [](auto x) { return -x; });
    return res;
}

template <typename T, int N, int M>
constexpr bool operator==(const mat<T, N, M> &lhs, const mat<T, N, M> &rhs) {
    return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
}

template <typename T, int N, int M>
constexpr mat<T, N, M> &operator+=(mat<T, N, M> &lhs, const mat<T, N, M> &rhs) {
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), lhs.begin(),
                   std::plus{});
    return lhs;
}

template <typename T, int N, int M>
constexpr mat<T, N, M> &operator-=(mat<T, N, M> &lhs, const mat<T, N, M> &rhs) {
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), lhs.begin(),
                   std::minus{});
    return lhs;
}

template <typename T, int N, int M>
constexpr mat<T, N, M> operator+(const mat<T, N, M> &lhs,
                                 const mat<T, N, M> &rhs) {
    mat<T, N, M> res;
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                   std::plus{});
    return res;
}

template <typename T, int N, int M>
constexpr mat<T, N, M> operator-(const mat<T, N, M> &lhs,
                                 const mat<T, N, M> &rhs) {
    mat<T, N, M> res;
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                   std::minus{});
    return res;
}

//
// Operation with a scalar : v2 = scalar `op` v
// Note: There is no operator- (because i want the usual order
// scalar `op` to be respected), use resp. (-x)*v
//

template <typename T, int N, int M>
constexpr mat<T, N, M> operator+(const typename mat<T, N, M>::value_type scalar,
                                 const mat<T, N, M> &rhs) {
    mat<T, N, M> res;
    std::transform(rhs.cbegin(), rhs.cend(), res.begin(),
                   [scalar](const auto &rhs_) { return scalar + rhs_; });
    return res;
}
template <typename T, int N, int M>
constexpr mat<T, N, M> operator*(const typename mat<T, N, M>::value_type scalar,
                                 const mat<T, N, M> &rhs) {
    mat<T, N, M> res;
    std::transform(rhs.cbegin(), rhs.cend(), res.begin(),
                   [scalar](const auto &rhs_) { return scalar * rhs_; });
    return res;
}

template <typename T, int N, int M, int L>
constexpr mat<T, N, L> operator*(const mat<T, N, M> &rhs,
                                 const mat<T, M, L> &lhs) {
    return dot<T, N, M, L>(rhs, lhs);
}

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

template <typename T, int N, int M>
constexpr mat<T, N, M> componentwise_product(const mat<T, N, M> &lhs,
                                             const mat<T, N, M> &rhs) {
    mat<T, N, M> res;
    std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                   std::multiplies{});
    return res;
}

// vector specific functions

template <typename T, int N>
constexpr typename vec<T, N>::value_type dot(const vec<T, N> &lhs,
                                             const vec<T, N> &rhs) {
    const mat tmp = componentwise_product(lhs, rhs);
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


// HACKY!
// Use the fact that vec is a POD containing only an array of size N 
// thus we can use the storage of v as the storage of a smaller view
template <typename T, int N, int M, typename=std::enable_if_t<(N > M)>>
const vec<T, M>& get_view(vec<T, N> &v){
    const vec<T, M>& p = *((vec<T, M>*) &v); 
    return p;
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
