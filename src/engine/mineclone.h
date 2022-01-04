#ifndef MINECLONE_H_
#define MINECLONE_H_

#include <glad/glad.h>
// must be after glad
#include <GLFW/glfw3.h>

#include <optional>

#include "system/event_manager.h"
#include "system/renderer.h"
#include "utils/logging.h"

class Mineclone {
   public:
    Mineclone() /* throw */;
    ~Mineclone();
    void run();

   private:
    // Allow to delay initialisation
    std::optional<Renderer> renderer;
    GLFWwindow *mWindow = nullptr;
    Logger log{Logger::get("Mineclone")};
};

#endif  // !MINECLONE_H_
