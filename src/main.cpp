#include <iostream>
#include <string>

#include "engine/mineclone.h"
#include "engine/utils/logging.h"
#include "engine/utils/profiler.h"

int main(int /*unused*/, char** /*unused*/) {
    Logger::set_global_log_level(LogLevel::Trace);

    PROFILE_SCOPED();
    Mineclone game;
    game.run();
}
