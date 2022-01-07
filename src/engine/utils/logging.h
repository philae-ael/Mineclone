#ifndef LOGGING_H_
#define LOGGING_H_

#include <algorithm>
#include <chrono>
#include <compare>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_set>
#include <utility>

#include "logging_utils.h"

enum class LogLevel { Trace, Info, Warning, Error };

inline std::strong_ordering operator<=>(LogLevel l1, LogLevel l2) {
    return (int)l1 <=> (int)l2;
}

// Can I return that ? I hope strings are placed in .data
inline const char* logColor(LogLevel lvl) {
    if (!inTerminalOut()) return "";
    switch (lvl) {
        case LogLevel::Trace:
            return "";
            break;
        case LogLevel::Info:
            return "\033[34m";  // Blue
            break;
        case LogLevel::Warning:
            return "\033[33m";  // Yellow
            break;
        case LogLevel::Error:
            return "\033[31m";  // Red
            break;
    }
    return "";
}

inline const char* logResetColor() {
    if (!inTerminalOut()) return "";
    return "\033[0m";
}

struct LoggerOptions {
    const std::string category = "debug";
    LogLevel level = LogLevel::Trace;
    bool show_location = true;
    std::optional<std::source_location> location{};
};

class Logger {
    // There should only be one instance used at anytime
    class LoggerStream {
       public:
        LoggerStream& operator<<(LogLevel lvl) {
            log_level = lvl;
            return *this;
        }

        LoggerStream& operator<<(auto rhs) {
            // if it's an error, we want to be warned.
            // otherwise, we respect the category whitelist and the current
            // log_level
            if (log_level == LogLevel::Error ||
                (Logger::category_whitelist.contains(category) &&
                 log_level >= minimum_log_level)) {
                buf << logColor(log_level) << rhs << logResetColor();
            }
            return *this;
        }

        ~LoggerStream() {
            const std::string out = buf.str();
            if (out.empty()) return;

            const std::ios::fmtflags stream_flags{stream.flags()};

            auto time_end = std::chrono::system_clock::now();
            std::chrono::duration<float, std::milli> duration =
                time_end - Logger::time_begin;

            stream << std::fixed << std::setw(6) << duration.count() << "ms "
                   << buf.str() << std::endl;
            stream.flags(stream_flags);
        }

        LoggerStream(std::ostream& stream, LogLevel minimum_log_level,
                     const std::string& category)
            : log_level(minimum_log_level),
              minimum_log_level(minimum_log_level),
              stream(stream),
              category(category) {}

        LoggerStream(LoggerStream&& l) noexcept
            : log_level(l.log_level),
              minimum_log_level(l.minimum_log_level),
              stream(l.stream),
              category(l.category) {}

       private:
        LogLevel log_level;
        const LogLevel minimum_log_level;
        std::ostringstream buf;
        std::ostream& stream;
        const std::string& category;  // Can be a const ref, bc his father,
                                      // logger has to outlive LoggerStream
    };

   public:
    Logger(std::string log_string, LogLevel log_level = LogLevel::Trace,
           std::string category = "", std::ostream& stream = std::cout)
        : minimum_log_level(log_level),
          stream(stream),
          log_string(std::move(log_string)),
          category(std::move(category)) {}

    Logger(Logger& l) = default;
    Logger(Logger&& l) = default;

    LoggerStream operator<<(LogLevel lvl) {
        LoggerStream s{stream, std::max(global_log_level, minimum_log_level),
                       category};
        s << lvl << log_string;
        return s;
    };

    LoggerStream operator<<(auto rhs) {
        LoggerStream s{stream, std::max(global_log_level, minimum_log_level),
                       category};
        s << log_string << rhs;
        return s;
    };

    LoggerStream get_stream() {
        LoggerStream s{stream, std::max(global_log_level, minimum_log_level),
                       category};
        s << log_string;
        return s;
    };

    static void set_global_log_level(LogLevel lvl) { global_log_level = lvl; }

    static Logger get(
        LoggerOptions opts = {},
        std::source_location location = std::source_location::current());

    static void whitelist(std::unordered_set<std::string>& s) {
        category_whitelist.merge(s);
    }

    static void blacklist(const std::unordered_set<std::string>& s) {
        std::unordered_set<std::string> res;
        std::set_difference(category_whitelist.cbegin(),
                            category_whitelist.cend(), s.cbegin(), s.cend(),
                            std::inserter(res, res.end()));
        category_whitelist = res;
    }

    static std::unordered_set<std::string> category_whitelist;
    static std::unordered_set<std::string> category_known;

   private:
    static LogLevel global_log_level;
    LogLevel minimum_log_level;
    std::ostream& stream;
    const std::string log_string;
    const std::string category;

    static std::chrono::time_point<std::chrono::system_clock> time_begin;

    friend LoggerStream;
};

#endif  // !LOGGING_H_
