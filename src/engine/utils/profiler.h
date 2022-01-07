#ifndef PROFILER_H_
#define PROFILER_H_

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <ios>
#include <new>
#include <ostream>
#include <string>

// source_location is assured to be included there
#include "logging.h"

#define CAT2(a, b) a##b
#define CAT(a, b) CAT2(a, b)
#define UNIQUE_VAR() CAT(CAT(profiler_tmp_, __COUNTER__), __LINE__)
#define PROFILE_SCOPED_FPS() \
    auto UNIQUE_VAR() = ProfileFunction::make_profiler_scope(true)
#define PROFILE_SCOPED() \
    auto UNIQUE_VAR() = ProfileFunction::make_profiler_scope(false)

class ProfileFunction {
   public:
    ~ProfileFunction() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::micro> delta_us = end - begin;

        std::string suffix = "µs";
        double count = delta_us.count();
        if (count > 1000) {  // NOLINT
            count = std::chrono::duration<float, std::milli>(delta_us).count();
            suffix = "ms";
        }
        if (count > 1000) {  // NOLINT
            count = std::chrono::duration<float>(delta_us).count();
            suffix = "s";
        }

        auto s = logger.get_stream();
        s << std::fixed << std::setprecision(2) << count << suffix;
        if (show_fps) {
            int fps = static_cast<int>(
                1. / std::chrono::duration<float>(delta_us).count());

            s << " or " << fps << "fps";
        }
    }

    static ProfileFunction make_profiler_scope(
        bool show_fps = false,
        const std::source_location location = std::source_location::current()) {
        return {Logger::get({.category = "Profiler",
                             .level = LogLevel::Trace,
                             .location = location}),
                show_fps};
    }

   private:
    ProfileFunction(Logger l, bool show_fps)
        : begin(std::chrono::high_resolution_clock::now()),
          show_fps(show_fps),
          logger(std::move(l)) {}

    const std::chrono::high_resolution_clock::time_point begin;
    bool show_fps;
    Logger logger;
};

#endif  // !PROFILER_H_
