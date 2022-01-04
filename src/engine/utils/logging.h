#ifndef LOGGING_H_
#define LOGGING_H_

#include <compare>
#include <iostream>
#include <sstream>
#include <string>

#if __cpp_lib_source_location >= 201907L
#include <source_location>
#elif __has_include(<experimental/source_location>)
#include <experimental/source_location>
namespace std {
using experimental::source_location;
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

class Logger {
   public:
    Logger(std::string log_string, LogLevel log_level = LogLevel::Trace,
           std::ostream& stream = std::cout)
        : minimum_log_level(log_level),
          stream(stream),
          log_string(std::move(log_string)) {}
    Logger(Logger&& l) noexcept
        : minimum_log_level(l.minimum_log_level),
          stream(l.stream),
          log_string(l.log_string) {}

    // There should only be one instance used at anytime
    class LoggerStream {
       public:
        LoggerStream& operator<<(LogLevel lvl) {
            log_level = lvl;
            return *this;
        }

        LoggerStream& operator<<(auto rhs) {
            if (log_level >= minimum_log_level)
                stream << logColor(log_level) << rhs << logResetColor();
            return *this;
        }
        ~LoggerStream() {
            stream << "\n";
            stream.flags(stream_flags);
        }

        LoggerStream(LoggerStream& l) =
            delete;  // This class should ONLY be lvalue
        LoggerStream& operator=(LoggerStream& l) = delete;

       private:
        LoggerStream(std::ostream& stream, LogLevel minimum_log_level)
            : log_level(minimum_log_level),
              minimum_log_level(minimum_log_level),
              stream(stream) {
            stream_flags = stream.flags();
        }
        LoggerStream(LoggerStream&& l) noexcept : stream(l.stream) {}
        LogLevel log_level;
        LogLevel minimum_log_level;
        std::ostream& stream;
        std::ios::fmtflags stream_flags;

        friend Logger;
    };

    LoggerStream operator<<(LogLevel lvl) {
        LoggerStream s{stream, std::max(global_log_level, minimum_log_level)};
        s << lvl << log_string;
        return s;
    };
    LoggerStream operator<<(auto rhs) {
        LoggerStream s{stream, std::max(global_log_level, minimum_log_level)};
        s << log_string << rhs;
        return s;
    };

    static void set_global_log_level(LogLevel lvl) { global_log_level = lvl; }

    static Logger get(
        LogLevel lvl = LogLevel::Trace, const std::string& category = "",
        const std::source_location location = std::source_location::current()) {
        std::ostringstream ss;
        ss << "[" << category << (category.empty() ? "" : ": ")
           << location.function_name() << ":" << location.line() << "] ";
        return {ss.str(), lvl};
    }

   private:
    LogLevel minimum_log_level;
    static LogLevel global_log_level;
    std::ostream& stream;
    const std::string log_string;
};

#endif  // !LOGGING_H_
