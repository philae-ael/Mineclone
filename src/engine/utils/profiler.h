#ifndef PROFILER_H_
#define PROFILER_H_

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ios>
#include <new>
#include <ostream>
#include <source_location>
#include <string>

#include "logging.h"

#define CAT2(a, b) a##b
#define CAT(a, b) CAT2(a, b)
#define UNIQUE_VAR() CAT(CAT(profiler_tmp_, __COUNTER__), __LINE__)
#define PROFILE_SCOPED() \
    auto UNIQUE_VAR() = ProfileFunction::make_profiler_scope()

class ProfileFunction {
   public:
    ~ProfileFunction() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::micro> delta = end - begin;

        std::string suffix = "µs";
        double count = delta.count();
        if (count > 1000) {  // NOLINT
            count = std::chrono::duration<float, std::milli>(delta).count();
            suffix = "ms";
        }
        if (count > 1000) {  // NOLINT
            count = std::chrono::duration<float>(delta).count();
            suffix = "s";
        }

        logger << std::fixed << std::setprecision(2) << count
               << suffix;  // NOLINT
    }

    static ProfileFunction make_profiler_scope(
        const std::source_location location = std::source_location::current()) {
        return {Logger::get(LogLevel::Trace, "Profiler", location)};
    }

   private:
    ProfileFunction(Logger l)
        : begin(std::chrono::high_resolution_clock::now()),
          logger(std::move(l)) {}

    const std::chrono::high_resolution_clock::time_point begin;
    Logger logger;
};

#endif  // !PROFILER_H_
