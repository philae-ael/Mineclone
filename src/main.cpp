#include <iostream>
#include <string>

#include "engine/mineclone.h"
#include "engine/utils/logging.h"
#include "engine/utils/profiler.h"

int main(int argc, char** argv) {
    PROFILE_SCOPED();
    Logger::set_global_log_level(LogLevel::Trace);

    std::unordered_set<std::string> whitelist;
    std::unordered_set<std::string> blacklist;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if(argv[i][0] == '-')
                blacklist.emplace(argv[i] + 1);
            else
                whitelist.emplace(argv[i]);
        }
    } else
        whitelist = {"Mineclone"};
    Logger::whitelist(whitelist);
    Logger::blacklist(blacklist);

    Mineclone game;
    game.run();
}
