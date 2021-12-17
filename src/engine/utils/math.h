#ifndef MATH_H_
#define MATH_H_

#include <algorithm>
#include <functional>
#include <numeric>
namespace math {

template <typename T, int N>
struct vec {
    using value_type = T;
    using reference = value_type &;
    using const_reference = const value_type &;
    using pointer = value_type *;
    using iterator = value_type *;
    using const_iterator = const value_type *;
    value_type data[N];  // NOLINT: better than wrapping std::array

    constexpr reference operator[](const int i) { return data[i]; }
    constexpr const_reference operator[](int i) const { return data[i]; }

    constexpr const_iterator cbegin() const { return iterator(data); }
    constexpr const_iterator cend() const { return iterator(data + N); }
    constexpr iterator begin() { return iterator(data); }
    constexpr iterator end() { return iterator(data + N); }

    constexpr vec &operator-() {
        std::transform(this->cbegin(), this->cend(), this->begin(),
                       [](auto x) { return -x; });
        return *this;
    }

    constexpr friend bool operator==(const vec<T, N> &lhs,
                                     const vec<T, N> &rhs) {
        return std::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    constexpr vec &operator+=(const vec &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::plus{});
        return *this;
    }

    constexpr vec &operator-=(const vec &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::minus{});
        return *this;
    }

    constexpr vec &operator*=(const vec &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::multiplies{});
        return *this;
    }

    constexpr vec &operator/=(const vec &rhs) {
        std::transform(this->cbegin(), this->cend(), rhs.cbegin(),
                       this->begin(), std::divides{});
        return *this;
    }

    constexpr vec friend operator*(const vec<T, N> &lhs, const vec<T, N> &rhs) {
        vec res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::multiplies{});
        return res;
    }

    constexpr vec friend operator/(const vec<T, N> &lhs, const vec<T, N> &rhs) {
        vec res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::divides{});
        return res;
    }

    constexpr vec friend operator+(const vec<T, N> &lhs, const vec<T, N> &rhs) {
        vec res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::plus{});
        return res;
    }

    constexpr vec friend operator-(const vec<T, N> &lhs, const vec<T, N> &rhs) {
        vec res;
        std::transform(lhs.cbegin(), lhs.cend(), rhs.cbegin(), res.begin(),
                       std::minus{});
        return res;
    }

    constexpr value_type norm2() const { return dot(*this, *this); }

    constexpr bool near(const vec &other, value_type eps) const {
        return (*this - other)->norm2() < eps;
    }
};

template <typename T, int N>
constexpr typename vec<T, N>::value_type dot(const vec<T, N> &lhs,
                                             const vec<T, N> &rhs) {
    const vec tmp = lhs * rhs;
    return std::accumulate(tmp.cbegin(), tmp.cend(),
                           typename vec<T, N>::value_type{});
}

template <typename T>
constexpr vec<T, 3> wedge(const vec<T, 3> &lhs, const vec<T, 3> &rhs) {
    return {lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]};
};

using vec3f = vec<float, 3>;
using vec4f = vec<float, 4>;
using vec3d = vec<double, 3>;
using vec4d = vec<double, 4>;
using vec3i = vec<int, 3>;
using vec4i = vec<int, 4>;

}  // namespace math
#endif  // !MATH_H_
