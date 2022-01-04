#include "logging.h"

LogLevel Logger::global_log_level = LogLevel::Info;
std::unordered_set<std::string> Logger::category_whitelist{};
