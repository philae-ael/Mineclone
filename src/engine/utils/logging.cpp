#include "logging.h"

LogLevel Logger::global_log_level = LogLevel::Info;
std::unordered_set<std::string> Logger::category_whitelist{};
std::unordered_set<std::string> Logger::category_known{};
std::chrono::time_point<std::chrono::system_clock> Logger::time_begin{
    std::chrono::system_clock::now()};
Logger Logger::get(LoggerOptions opts, std::source_location location) {
    std::ostringstream ss;
    if (!opts.location.has_value()) opts.location = location;

    if (!category_known.contains(opts.category)) {
        // Order is important to prevent infinite recursion !
        category_known.insert(opts.category);
        get({"Categories", LogLevel::Info, false})
            << "New category " << opts.category;
    }
    ss << "[" << opts.category;
    if (opts.show_location)
        ss << (opts.category.empty() ? "" : ": ")
           << opts.location->function_name() << ":" << opts.location->line();

    ss << "] ";
    return {ss.str(), opts.level, opts.category};
}
