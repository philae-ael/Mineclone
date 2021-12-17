#include "mineclone.h"

#include <glad/glad.h>
// must be after glad
#include <GLFW/glfw3.h>

#include <exception>
#include <iostream>
#include "system/renderer.h"

Mineclone::Mineclone() {
    enum class WinInitError { WE_SUCCESS, WE_CREATION };
    auto createWindow = [this]() -> WinInitError {
        const int win_width = 600;
        const int win_height = 500;
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
        mWindow = glfwCreateWindow(win_width, win_height, "Mineclone", nullptr,
                                   nullptr);
        if (mWindow == nullptr) return WinInitError::WE_CREATION;

        glfwMakeContextCurrent(mWindow);
        gladLoadGL();
        std::cout << "Opengl version: " << glGetString(GL_VERSION) << "\n";
        return WinInitError::WE_SUCCESS;
    };

    WinInitError error = createWindow();
    switch (error) {
        case WinInitError::WE_CREATION:
            std::cout << "Error while creating window, Bye!\n";
            throw std::runtime_error("Error while creating window");
            break;
        case WinInitError::WE_SUCCESS:
            break;
    }

    renderer.emplace(); 
}

Mineclone::~Mineclone() { glfwTerminate(); }

void Mineclone::run() const {
    while (!glfwWindowShouldClose(mWindow)) {
        renderer->render();

        if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(mWindow, true);

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }
}
