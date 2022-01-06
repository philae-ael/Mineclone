#include "logging.h"

LogLevel Logger::global_log_level = LogLevel::Info;
std::unordered_set<std::string> Logger::category_whitelist{};
std::unordered_set<std::string> Logger::category_known{};
std::chrono::time_point<std::chrono::system_clock> Logger::time_begin{
    std::chrono::system_clock::now()};
