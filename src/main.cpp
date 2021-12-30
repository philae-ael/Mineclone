#include <iostream>
#include <string>

#include "engine/mineclone.h"
#include "engine/utils/logging.h"

int main(int /*unused*/, char** /*unused*/) {
    Logger::set_global_log_level(LogLevel::Info);

    Mineclone game;
    game.run();
}
