#ifndef MINECLONE_H_
#define MINECLONE_H_

#include <glad/glad.h>
// must be after glad
#include <GLFW/glfw3.h>
#include <optional>

#include "system/renderer.h"

class Mineclone {
   public:
    Mineclone() /* throw */;
    ~Mineclone();
    void run() const;

   private:
    // Allow to delay initialisation
    std::optional<Renderer> renderer;
    GLFWwindow *mWindow = nullptr;
};

#endif  // !MINECLONE_H_
