#ifndef CONSTEXPR_FUNCTIONS_H_
#define CONSTEXPR_FUNCTIONS_H_

#include <cmath>
#include <type_traits>

namespace math {

// Some constexpr versions of common math functions
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

}  // namespace math
#endif  // !CONSTEXPR_FUNCTIONS_H_
