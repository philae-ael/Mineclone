#include <iostream>
#include <string>

#include "engine/mineclone.h"
#include "engine/utils/logging.h"
#include "engine/utils/profiler.h"

int main(int argc, char** argv) {
    PROFILE_SCOPED();
    Logger::set_global_log_level(LogLevel::Trace);
    Logger log = Logger::get("Main");

    std::unordered_set<std::string> whitelist{"Main", "debug"};
    std::unordered_set<std::string> blacklist;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (argv[i][0] == '-')
                blacklist.emplace(argv[i] + 1);
            else
                whitelist.emplace(argv[i]);
        }
    }
    Logger::whitelist(whitelist);
    Logger::blacklist(blacklist);

    log << LogLevel::Info << "logger categories enabled "
        << Logger::category_whitelist;

    Mineclone game;
    game.run();
}
