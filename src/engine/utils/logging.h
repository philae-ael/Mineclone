#ifndef LOGGING_H_
#define LOGGING_H_

#include <compare>
#include <iostream>
#include <sstream>
#include <string>

#define LOG log_(__FILE__, __func__)

enum class LogLevel { Trace, Info, Warning, Error };

inline std::strong_ordering operator<=>(const LogLevel& l1,
                                        const LogLevel& l2) {
    return (const int)l1 <=> (const int)l2;
}

// Can I return that ? I hope strings are placed in .data
inline const char* logColor(LogLevel lvl) {
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
}

inline const char* logResetColor() { return "\033[0m"; }

class Logger {
   public:
    Logger(std::string log_string, std::ostream* stream,
           LogLevel log_level = LogLevel::Info)
        : minimum_log_level(log_level),
          stream(stream),
          log_string(std::move(log_string)) {}
    Logger(Logger&& l) noexcept {
        stream = l.stream;
        l.stream = nullptr;
    }

    // There should only be one instance used at anytime
    class LoggerStream {
       public:
        LoggerStream(std::ostream* stream, LogLevel log_level)
            : minimum_log_level(log_level), stream(stream) {
            if (stream) stream_flags = stream->flags();
        }
        LoggerStream(LoggerStream& l) =
            delete;  // This class should ONLY be lvalue
        LoggerStream& operator=(LoggerStream& l) = delete;
        LoggerStream(LoggerStream&& l) noexcept {
            stream = l.stream;
            l.stream = nullptr;
        }

        LoggerStream& operator<<(LogLevel lvl) {
            log_level = lvl;
            return *this;
        }

        LoggerStream& operator<<(auto rhs) {
            if (stream && log_level >= minimum_log_level)
                *stream << logColor(log_level) << rhs << logResetColor();
            return *this;
        }
        ~LoggerStream() {
            (*this) << "\n";
            if (stream) stream->flags(stream_flags);
        }

       private:
        LogLevel log_level;
        LogLevel minimum_log_level;
        std::ostream* stream = nullptr;
        std::ios::fmtflags stream_flags;
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

    static void set_global_log_level(LogLevel lvl){
        global_log_level = lvl;
    }

   private:
    LogLevel minimum_log_level;
    static LogLevel global_log_level;
    std::ostream* stream = nullptr;
    const std::string log_string;
};

inline Logger log_(const char* file, const char* fnc) {
    std::ostringstream ss;
    ss << "[" << file << ":" << fnc << "] ";
    return {ss.str(), &std::cout};
}

#endif  // !LOGGING_H_
