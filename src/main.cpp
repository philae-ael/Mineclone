#include <iostream>
#include <string>

#include "engine/mineclone.h"
#include "engine/utils/logging.h"
#include "engine/utils/profiler.h"

int main(int argc, char** argv) {
    Logger::set_global_log_level(LogLevel::Trace);

    std::unordered_set<std::string> whitelist;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            whitelist.emplace(argv[i]);
        }
    } else
        whitelist = {"Mineclone"};
    Logger::whitelist(whitelist);

    PROFILE_SCOPED();
    Mineclone game;
    game.run();
}
