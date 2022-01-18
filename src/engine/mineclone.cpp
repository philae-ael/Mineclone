#include "mineclone.h"

#include <glad/glad.h>
// must be after glad
#include <GLFW/glfw3.h>
#include <bindings/imgui_impl_glfw.h>
#include <bindings/imgui_impl_opengl3.h>
#include <imgui.h>

#include <chrono>
#include <exception>
#include <iostream>

#include "data/camera_event.h"
#include "system/event_manager.h"
#include "system/renderer.h"
#include "utils/logging.h"

Mineclone::Mineclone() {
    enum class WinInitError { WE_SUCCESS, WE_CREATION };
    const int win_width = 800;
    const int win_height = 600;
    auto createWindow = [&]() -> WinInitError {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
        mWindow = glfwCreateWindow(win_width, win_height, "Mineclone", nullptr,
                                   nullptr);
        if (mWindow == nullptr) return WinInitError::WE_CREATION;

        glfwMakeContextCurrent(mWindow);
        glfwSwapInterval(0);

        gladLoadGL();
        log << LogLevel::Info << "Opengl version: " << glGetString(GL_VERSION)
            << "\n";
        return WinInitError::WE_SUCCESS;
    };

    WinInitError error = createWindow();
    switch (error) {
        case WinInitError::WE_CREATION:
            log << LogLevel::Error << "Error while creating window, Bye!\n";
            throw std::runtime_error("Error while creating window");
            break;
        case WinInitError::WE_SUCCESS:
            break;
    }

    renderer = std::make_unique<Renderer>();

    {  // Set Resize Callback
        auto resize_callback = [](GLFWwindow* window, int width, int height) {
            // Can't bind this the normal way bc we need to convert the lambda
            // to void (*)(GLFWindow*,int,int)
            auto* self = (Mineclone*)glfwGetWindowUserPointer(window);
            glViewport(0, 0, width, height);
            self->renderer->setWindowSize(width, height);
        };

        glfwSetWindowUserPointer(mWindow, this);
        glfwSetFramebufferSizeCallback(mWindow, resize_callback);

        resize_callback(mWindow, win_width, win_height);
    }

    {  // Setup ImGUI
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
        ImGui_ImplOpenGL3_Init("#version 450");
    }
}

Mineclone::~Mineclone() {
    // We do it this way so that glfwTerminate is called after deleting Renderer
    renderer.reset();

    {  // Terminate IMGUI
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    glfwTerminate();
}

void convertDispatchEvent(GLFWwindow* mWindow) {
    CameraMoveEvent ev{};

    // Camera translation with WASD + Space/Shift
    if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
        ev.translationAxis[0] -= 1;
    if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
        ev.translationAxis[0] += 1;
    if (glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        ev.translationAxis[1] -= 1;
    if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
        ev.translationAxis[1] += 1;
    if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
        ev.translationAxis[2] -= 1;
    if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
        ev.translationAxis[2] += 1;

    // Camera arrows with the arrows
    if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) 
        ev.rotationAxis[0] += 1;
    if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
        ev.rotationAxis[0] -= 1;
    if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
        ev.rotationAxis[1] += 1;
    if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
        ev.rotationAxis[1] -= 1;

    EventManager::dispatch(ev);
}

void runImGui(GLFWwindow* mWindow) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("ToolBar");
    if (ImGui::Button("Exit")) glfwSetWindowShouldClose(mWindow, true);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Mineclone::run() {
    auto begin = std::chrono::high_resolution_clock::now();

    while (!glfwWindowShouldClose(mWindow)) {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> frame_duration = end - begin;

        convertDispatchEvent(mWindow);
        renderer->update(frame_duration.count());
        renderer->render();

        runImGui(mWindow);

        glfwSwapBuffers(mWindow);
        glfwPollEvents();

        begin = end;
    }
}
