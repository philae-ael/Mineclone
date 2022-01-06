#ifndef LOGGING_UTILS_H_
#define LOGGING_UTILS_H_

#include <array>
#include <iomanip>
#include <iostream>
#include <unordered_set>

#include "mat.h"

#if __cpp_lib_source_location >= 201907L
#include <source_location>
#elif __has_include(<experimental/source_location>)
#include <experimental/source_location>
#include <utility>
namespace std {
using experimental::source_location;  // NOLINT
};
#else
#error "can't find source_location header!"
#endif

#ifdef __unix__
#include <unistd.h>
inline bool inTerminalOut() { return isatty(fileno(stdout)); }
#else
inline bool inTerminalOut() { return false; }
#endif

namespace nostd {
template <class T>
struct is_container_h : std::false_type {};

template <class T, int N, int M>
struct is_container_h<math::mat<T, N, M>> : std::true_type {};
template <class T>
struct is_container_h<std::unordered_set<T>> : std::true_type {};
template <class T, std::size_t N>
struct is_container_h<std::array<T, N>> : std::true_type {};

template <class T>
struct is_container : is_container_h<std::remove_cv_t<T>> {};

template <class T>
constexpr bool is_container_v = is_container<T>::value;

template <class T>
struct rank;

template <class T>
struct rank_h : std::rank<T> {};

template <class T, int N, int M>
struct rank_h<math::mat<T, N, M>>
    : std::integral_constant<std::size_t, M == 1 ? 2 : 1> {};

template <class T, std::size_t N>
struct rank_h<std::array<T, N>>
    : std::integral_constant<std::size_t, 1 + rank<T>::value> {};

template <class T>
struct rank : rank_h<std::remove_cv_t<T>> {};

template <class T>
constexpr bool rank_v = is_container<T>::value;
};  // namespace nostd

// std::set, array, math::mat<T, N, M > 1>
template <class T, std::enable_if_t<nostd::is_container_v<T>, bool> = true,
          std::enable_if_t<nostd::rank_v<T> == 1, bool> = true>
std::ostream& operator<<(std::ostream& s, const T& cont) {
    s << "[";
    bool first = true;
    for (auto& a : cont) {
        if (!first) s << " ";
        s << a;
        first = false;
    }
    s << "]";
    return s;
}

template <class T, std::enable_if_t<nostd::is_container_v<T>, bool> = true,
          std::enable_if_t<nostd::rank_v<T> == 2> = true, bool>
std::ostream& operator<<(std::ostream& s, const T& arr) {
    s << "\n[";
    for (auto& a : arr) s << a << "\n";
    s << "]";
    return s;
}

// math::mat<T, N, 1>
template <typename T, int N,
          std::enable_if_t<std::is_integral_v<T>, bool> = true>
std::ostream& operator<<(std::ostream& s, const math::mat<T, N, 1>& m) {
    s << "[";
    bool first = true;
    for (auto& a : m) {
        if (!first) s << " ";
        s << std::setw(3) << a;
        first = false;
    }
    s << "]";
    return s;
}

template <typename T, int N,
          std::enable_if_t<std::is_floating_point_v<T>, bool> = true>
std::ostream& operator<<(std::ostream& s, const math::mat<T, N, 1>& m) {
    s << "[";
    bool first = true;
    for (auto& a : m) {
        if (!first) s << " ";
        s << std::setprecision(2) << a;
        first = false;
    }
    s << "]";
    return s;
}

#endif  // LOGGING_UTILS_H_
