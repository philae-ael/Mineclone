#ifndef MINECLONE_H_
#define MINECLONE_H_

#include <glad/glad.h>
// must be after glad
#include <GLFW/glfw3.h>

class Mineclone {
   public:

    // Will throw if creation of the window failed
    Mineclone();

    ~Mineclone();
    void run() const;
   private:
    GLFWwindow* mWindow = nullptr;
};

#endif  // !MINECLONE_H_
